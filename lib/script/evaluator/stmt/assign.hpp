#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"

namespace valkey::script {

ExecResult Exec(const AssignStmt& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto value, Eval(node.value, ctx));
    if (!ctx.env.Assign(node.name, std::move(value))) {
        return MakeError(ErrKind::kName, "assignment to undefined variable '" + node.name + "'");
    }
    return Normal{};
}

}  // namespace valkey::script
