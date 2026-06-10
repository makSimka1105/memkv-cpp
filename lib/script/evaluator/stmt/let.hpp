#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"

namespace valkey::script {

ExecResult Exec(const LetStmt& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto value, Eval(node.init, ctx));
    ctx.env.Declare(node.name, std::move(value));
    return Normal{};
}

}  // namespace valkey::script
