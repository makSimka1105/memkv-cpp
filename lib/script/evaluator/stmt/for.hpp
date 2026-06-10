#pragma once

#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ExecResult Exec(const ForStmt& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto iterable, Eval(node.iter, ctx));
    const auto* list = std::get_if<Box<List>>(&iterable);
    if (!list) {
        return MakeError(ErrKind::kType, "for expects a list");
    }
    for (const auto& element : (*list)->items) {
        VK_RETURN_IF_ERROR(Tick(ctx));
        ScopeGuard guard(ctx.env);
        ctx.env.Declare(node.var, element);
        VK_ASSIGN_OR_RETURN(auto flow, Exec(*node.body, ctx));
        if (std::holds_alternative<Return>(flow)) {
            return flow;
        }
    }
    return Normal{};
}

}  // namespace valkey::script
