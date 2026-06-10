#pragma once

#include <cstddef>
#include <expected>
#include <variant>

#include "script/error.hpp"
#include "script/evaluator/core/env.hpp"
#include "script/value.hpp"

namespace valkey::engine {
class Engine;
}  // namespace valkey::engine

namespace valkey::script {

struct EvalContext {
    Env& env;
    engine::Engine& engine;
    size_t steps = 0;
    size_t step_limit = 0;
};

struct Normal {};
struct Return {
    ScriptValue value;
};

using Flow = std::variant<Normal, Return>;
using ExecResult = std::expected<Flow, ScriptError>;

std::expected<void, ScriptError> Tick(EvalContext& ctx) {
    if (++ctx.steps > ctx.step_limit) {
        return MakeError(ErrKind::kIterLimit, "script exceeded iteration limit");
    }
    return {};
}

class ScopeGuard {
public:
    explicit ScopeGuard(Env& env) : env_(env) { env_.EnterScope(); }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ~ScopeGuard() { env_.LeaveScope(); }

private:
    Env& env_;
};

}  // namespace valkey::script
