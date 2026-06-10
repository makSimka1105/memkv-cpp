#pragma once

#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"

namespace valkey::script {

ExecResult Exec(const Block& node, EvalContext& ctx) {
    ScopeGuard guard(ctx.env);
    for (const auto& statement : node.stmts) {
        VK_ASSIGN_OR_RETURN(auto flow, Exec(statement, ctx));
        if (std::holds_alternative<Return>(flow)) {
            return flow;
        }
    }
    return Normal{};
}

}  // namespace valkey::script
