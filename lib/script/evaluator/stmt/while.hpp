#pragma once

#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ExecResult Exec(const WhileStmt& node, EvalContext& ctx) {
    while (true) {
        VK_ASSIGN_OR_RETURN(auto condition, Eval(node.cond, ctx));
        if (!IsTruthy(condition)) {
            break;
        }
        VK_RETURN_IF_ERROR(Tick(ctx));
        VK_ASSIGN_OR_RETURN(auto flow, Exec(*node.body, ctx));
        if (std::holds_alternative<Return>(flow)) {
            return flow;
        }
    }
    return Normal{};
}

}  // namespace valkey::script
