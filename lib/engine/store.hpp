#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "engine/memory_tracker.hpp"
#include "meta/type_list.hpp"
#include "storage/storage.hpp"
#include "storage/value.hpp"
#include "types/all.hpp"

namespace valkey::engine {

class Store {
public:
    enum class TypeStatus { kOk, kMissing, kWrongType };
    enum class UpsertStatus { kOk, kWrongType, kOom };

    Store() { WireExpiry(); }
    Store(const Store&) = delete;
    Store& operator=(const Store&) = delete;
    Store(Store&& other) noexcept
        : storage_(std::move(other.storage_)), mem_(std::move(other.mem_)) {
        WireExpiry();
    }
    Store& operator=(Store&& other) noexcept {
        storage_ = std::move(other.storage_);
        mem_ = std::move(other.mem_);
        WireExpiry();
        return *this;
    }

    void SetMaxMemory(size_t bytes) { mem_.SetLimit(bytes); }
    size_t UsedMemory() const { return mem_.Used(); }
    size_t MaxMemory() const { return mem_.Limit(); }

    template <typename T>
    bool ForceSet(std::string key, T value) {
        const size_t old_bytes = ExistingBytes(key);
        const size_t new_bytes = ValBytes(key, value);
        if (new_bytes > old_bytes && mem_.WouldOverflow(new_bytes - old_bytes)) {
            return false;
        }
        mem_.Sub(old_bytes);
        mem_.Add(new_bytes);
        storage_.Set(std::move(key), std::move(value));
        return true;
    }

    bool Del(std::string_view key) {
        const auto* var = storage_.Get(key);
        if (var == nullptr) {
            return false;
        }
        mem_.Sub(std::visit([&](const auto& val) { return ValBytes(key, val); }, *var));
        return storage_.Del(key);
    }

    bool Exists(std::string_view key) { return storage_.Exists(key); }

    bool Expire(std::string_view key, std::chrono::seconds ttl) {
        return storage_.Expire(key, ttl);
    }

    void AdvanceClock(std::chrono::steady_clock::duration d) { storage_.AdvanceClock(d); }

    int64_t TtlSeconds(std::string_view key) { return storage_.TtlSeconds(key); }

    size_t DbSize() { return storage_.DbSize(); }

    void Flush() {
        storage_.FlushDb();
        mem_ = MemoryTracker{};
    }

    std::string_view TypeOf(std::string_view key) {
        const auto* var = storage_.Get(key);
        if (var == nullptr) {
            return "none";
        }
        return std::visit(
            [](const auto& val) -> std::string_view {
                using T = std::decay_t<decltype(val)>;
                return TraitForT<T, types::AllStoredTypes>::kName;
            },
            *var);
    }

    std::vector<std::string> Keys(std::string_view pattern) {
        return storage_.Keys(pattern);
    }

    size_t MemoryUsage(std::string_view key) { return ExistingBytes(key); }

    template <typename T>
    std::pair<const T*, TypeStatus> As(std::string_view key) {
        const auto* var = storage_.Get(key);
        if (var == nullptr) {
            return {nullptr, TypeStatus::kMissing};
        }
        const auto* t = std::get_if<T>(var);
        if (t == nullptr) {
            return {nullptr, TypeStatus::kWrongType};
        }
        return {t, TypeStatus::kOk};
    }

    template <typename T, typename Fn>
    TypeStatus Mutate(std::string_view key, Fn&& fn) {
        auto* var = storage_.GetMutable(key);
        if (var == nullptr) {
            return TypeStatus::kMissing;
        }
        auto* t = std::get_if<T>(var);
        if (t == nullptr) {
            return TypeStatus::kWrongType;
        }
        const size_t old_bytes = ValBytes(key, *t);
        std::forward<Fn>(fn)(*t);
        mem_.Sub(old_bytes);
        if (IsEmpty(*t)) {
            storage_.Del(key);
        } else {
            mem_.Add(ValBytes(key, *t));
        }
        return TypeStatus::kOk;
    }

    template <typename T, typename Fn>
    UpsertStatus Upsert(std::string key, Fn&& fn) {
        auto* var = storage_.GetMutable(key);
        if (var != nullptr) {
            auto* t = std::get_if<T>(var);
            if (t == nullptr) {
                return UpsertStatus::kWrongType;
            }
            const size_t old_bytes = ValBytes(key, *t);
            std::forward<Fn>(fn)(*t);
            mem_.Sub(old_bytes);
            mem_.Add(ValBytes(key, *t));
            return UpsertStatus::kOk;
        }
        T fresh{};
        std::forward<Fn>(fn)(fresh);
        const size_t need = ValBytes(key, fresh);
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

    template <class T>
    size_t ValBytes(std::string_view key, const T& val) {
        return StringBytes(key) + SizeOf(val);
    }

    void WireExpiry() {
        storage_.OnExpire([this](std::string_view key, const storage::ValueVar& v) {
            mem_.Sub(std::visit([&](const auto& val) { return ValBytes(key, val); }, v));
        });
    }

    size_t ExistingBytes(std::string_view key) {
        const auto* var = storage_.Get(key);
        if (var == nullptr) {
            return 0;
        }
        return std::visit([&](const auto& val) { return ValBytes(key, val); }, *var);
    }

    storage::Storage storage_;
    MemoryTracker mem_;
};

}  // namespace valkey::engine
