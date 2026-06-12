#pragma once

#include <array>
#include <cstdint>
#include <variant>

#include "meta/table.hpp"
#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

namespace un_detail {

using UnaryRule = ValueResult (*)(const UnOp&, EvalContext&);

ValueResult Negate(const UnOp& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto operand, Eval(*node.operand, ctx));
    if (const auto* whole = std::get_if<int64_t>(&operand)) {
        return -*whole;
    }
    if (const auto* real = std::get_if<double>(&operand)) {
        return -*real;
    }
    return MakeError(ErrKind::kType, "cannot negate a non-number");
}

ValueResult LogicalNot(const UnOp& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto operand, Eval(*node.operand, ctx));
    return !IsTruthy(operand);
}

constexpr std::array<UnaryRule, UnOp::kLast> BuildTable() {
    std::array<UnaryRule, UnOp::kLast> table{};
    table[UnOp::kNeg] = &Negate;
    table[UnOp::kNot] = &LogicalNot;
    return table;
}

constexpr auto kUnaryTable = BuildTable();
static_assert(AllSet(kUnaryTable), "UnOp evaluator table incomplete");

}  // namespace un_detail

ValueResult Eval(const UnOp& node, EvalContext& ctx) {
    return un_detail::kUnaryTable[node.op](node, ctx);
}

}  // namespace valkey::script
