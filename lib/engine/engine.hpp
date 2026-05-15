#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "engine/config.hpp"
#include "engine/memory_tracker.hpp"
#include "storage/storage.hpp"
#include "storage/value.hpp"

namespace valkey::engine {

class Engine {
public:
    enum class TypeStatus { kOk, kMissing, kWrongType };
    enum class UpsertStatus { kOk, kWrongType, kOom };

    explicit Engine(Config cfg = {}) : cfg_(cfg) { mem_.SetLimit(cfg_.maxmemory); }

    bool SetString(std::string key, std::string value) {
        const size_t old_bytes = ExistingBytes(key);
        const size_t new_bytes = StringBytes(key) + StringBytes(value);
        if (new_bytes > old_bytes && mem_.WouldOverflow(new_bytes - old_bytes)) {
            return false;
        }
        mem_.Sub(old_bytes);
        mem_.Add(new_bytes);
        storage_.Set(std::move(key), storage::String(std::move(value)));
        return true;
    }

    bool DelKey(std::string_view key) {
        const size_t bytes = ExistingBytes(key);
        if (bytes == 0) {
            return false;
        }
        mem_.Sub(bytes);
        return storage_.Del(key);
    }

    bool ExistsKey(std::string_view key) { return storage_.Get(key) != nullptr; }

    bool ExpireKey(std::string_view key, std::chrono::seconds ttl) {
        return storage_.Expire(key, std::chrono::steady_clock::now() + ttl);
    }

    int64_t TtlSeconds(std::string_view key) const {
        return static_cast<int64_t>(storage_.TtlSeconds(key));
    }

    size_t DbSize() const { return storage_.DbSize(); }
    size_t UsedMemory() const { return mem_.Used(); }
    size_t MaxMemory() const { return mem_.Limit(); }
    void SetMaxMemory(size_t bytes) {
        cfg_.maxmemory = bytes;
        mem_.SetLimit(bytes);
    }

    void FlushDb() {
        storage_.FlushDb();
        mem_ = MemoryTracker{};
        mem_.SetLimit(cfg_.maxmemory);
    }

    size_t SweepExpired() { return storage_.SweepExpired(); }

    std::string_view TypeOf(std::string_view key) {
        const auto* v = storage_.Get(key);
        if (v == nullptr) {
            return "none";
        }
        if (std::get_if<storage::String>(v)) {
            return "string";
        }
        if (std::get_if<storage::List>(v)) {
            return "list";
        }
        if (std::get_if<storage::Set>(v)) {
            return "set";
        }
        return "unknown";
    }

    std::vector<std::string> Keys(std::string_view pattern) { return storage_.Keys(pattern); }

    size_t MemoryUsage(std::string_view key) { return ExistingBytes(key); }

    template <typename T>
    std::pair<const T*, TypeStatus> As(std::string_view key) {
        const auto* v = storage_.Get(key);
        if (v == nullptr) {
            return {nullptr, TypeStatus::kMissing};
        }
        const auto* t = std::get_if<T>(v);
        if (t == nullptr) {
            return {nullptr, TypeStatus::kWrongType};
        }
        return {t, TypeStatus::kOk};
    }

    template <typename T, typename Fn>
    TypeStatus Mutate(std::string_view key, Fn&& fn) {
        auto* v = storage_.GetMutable(key);
        if (v == nullptr) {
            return TypeStatus::kMissing;
        }
        auto* t = std::get_if<T>(v);
        if (t == nullptr) {
            return TypeStatus::kWrongType;
        }
        const size_t old_bytes = StringBytes(key) + SizeOf(*t);
        std::forward<Fn>(fn)(*t);
        if (IsEmpty(*t)) {
            mem_.Sub(old_bytes);
            storage_.Del(key);
        } else {
            mem_.Sub(old_bytes);
            mem_.Add(StringBytes(key) + SizeOf(*t));
        }
        return TypeStatus::kOk;
    }

    template <typename T, typename Fn>
    UpsertStatus Upsert(std::string key, Fn&& fn) {
        auto* v = storage_.GetMutable(key);
        if (v != nullptr) {
            auto* t = std::get_if<T>(v);
            if (t == nullptr) {
                return UpsertStatus::kWrongType;
            }
            const size_t old_bytes = StringBytes(key) + SizeOf(*t);
            std::forward<Fn>(fn)(*t);
            mem_.Sub(old_bytes);
            mem_.Add(StringBytes(key) + SizeOf(*t));
            return UpsertStatus::kOk;
        }
        T fresh{};
        std::forward<Fn>(fn)(fresh);
        const size_t need = StringBytes(key) + SizeOf(fresh);
        if (mem_.WouldOverflow(need)) {
            return UpsertStatus::kOom;
        }
        mem_.Add(need);
        storage_.Set(std::move(key), std::move(fresh));
        return UpsertStatus::kOk;
    }

private:
    static constexpr size_t StringBytes(std::string_view sv) {
        return sizeof(std::string) + sv.size();
    }

    size_t ExistingBytes(std::string_view key) {
        const auto* v = storage_.Get(key);
        if (v == nullptr) {
            return 0;
        }
        return StringBytes(key) + SizeOf(*v);
    }

    Config cfg_;
    storage::Storage storage_;
    MemoryTracker mem_;
};

}  // namespace valkey::engine
