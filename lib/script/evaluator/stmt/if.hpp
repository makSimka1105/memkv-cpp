#pragma once

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ExecResult Exec(const IfStmt& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto condition, Eval(node.cond, ctx));
    if (IsTruthy(condition)) {
        return Exec(*node.then_, ctx);
    }
    if (node.else_) {
        return Exec(**node.else_, ctx);
    }
    return Normal{};
}

}  // namespace valkey::script
