#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ExecResult Exec(const ReturnStmt& node, EvalContext& ctx) {
    if (!node.value) {
        return Return{Nil{}};
    }
    VK_ASSIGN_OR_RETURN(auto value, Eval(*node.value, ctx));
    return Return{std::move(value)};
}

}  // namespace valkey::script
