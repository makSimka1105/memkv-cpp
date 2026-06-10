#pragma once

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"

namespace valkey::script {

ExecResult Exec(const ExprStmt& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto value, Eval(node.expr, ctx));
    (void)value;
    return Normal{};
}

}  // namespace valkey::script
