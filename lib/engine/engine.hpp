#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "engine/config.hpp"
#include "engine/script_registry.hpp"
#include "engine/store.hpp"
#include "io/reply.hpp"

namespace valkey::engine {

class Engine {
public:
    using TypeStatus = Store::TypeStatus;
    using UpsertStatus = Store::UpsertStatus;

    explicit Engine(Config cfg = {}) : cfg_(cfg) { store_.SetMaxMemory(cfg_.maxmemory); }

    template <typename T>
    bool ForceSet(std::string key, T value) {
        return store_.ForceSet<T>(std::move(key), std::move(value));
    }

    bool DelKey(std::string_view key) { return store_.Del(key); }
    bool ExistsKey(std::string_view key) { return store_.Exists(key); }

    bool ExpireKey(std::string_view key, std::chrono::seconds ttl) {
        return store_.Expire(key, ttl);
    }

    int64_t TtlSeconds(std::string_view key) { return store_.TtlSeconds(key); }

    void AdvanceClock(std::chrono::steady_clock::duration d) { store_.AdvanceClock(d); }

    size_t DbSize() { return store_.DbSize(); }
    size_t UsedMemory() const { return store_.UsedMemory(); }
    size_t MaxMemory() const { return store_.MaxMemory(); }

    void SetMaxMemory(size_t bytes) {
        cfg_.maxmemory = bytes;
        store_.SetMaxMemory(bytes);
    }

    void FlushDb() {
        store_.Flush();
        store_.SetMaxMemory(cfg_.maxmemory);
    }

    std::string_view TypeOf(std::string_view key) { return store_.TypeOf(key); }
    std::vector<std::string> Keys(std::string_view pattern) { return store_.Keys(pattern); }
    size_t MemoryUsage(std::string_view key) { return store_.MemoryUsage(key); }

    template <typename T>
    std::pair<const T*, TypeStatus> As(std::string_view key) {
        return store_.As<T>(key);
    }

    template <typename T, typename Fn>
    TypeStatus Mutate(std::string_view key, Fn&& fn) {
        return store_.Mutate<T>(key, std::forward<Fn>(fn));
    }

    template <typename T, typename Fn>
    UpsertStatus Upsert(std::string key, Fn&& fn) {
        return store_.Upsert<T>(std::move(key), std::forward<Fn>(fn));
    }

    void RegisterFunction(std::string name, ScriptFn fn) {
        scripts_.Register(std::move(name), std::move(fn));
    }

    const ScriptFn* FindFunction(std::string_view name) const { return scripts_.Find(name); }

    bool DeleteFunction(std::string_view name) { return scripts_.Delete(name); }

    size_t ScriptMaxIters() const { return cfg_.script_max_iters; }

    using RawEvalFn = std::function<io::Reply(const std::vector<std::string>&)>;

    void SetRawEvalFn(RawEvalFn fn) { raw_eval_fn_ = std::move(fn); }

    io::Reply EvalRaw(const std::vector<std::string>& args) {
        if (!raw_eval_fn_) {
            return io::MakeErr("ERR engine not configured for raw eval");
        }
        return raw_eval_fn_(args);
    }

private:
    Config cfg_;
    Store store_;
    ScriptRegistry scripts_;
    RawEvalFn raw_eval_fn_;
};

}  // namespace valkey::engine
