#pragma once

#include <cstdint>
#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const UnOp& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto operand, Eval(*node.operand, ctx));
    if (node.op == UnOp::kNot) {
        return !IsTruthy(operand);
    }
    if (const auto* whole = std::get_if<int64_t>(&operand)) {
        return -*whole;
    }
    if (const auto* real = std::get_if<double>(&operand)) {
        return -*real;
    }
    return MakeError(ErrKind::kType, "cannot negate a non-number");
}

}  // namespace valkey::script
