#pragma once

#include <array>
#include <cstdint>
#include <variant>

#include "meta/table.hpp"
#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

namespace bin_detail {

using BinaryRule = ValueResult (*)(const BinOp&, EvalContext&);

ValueResult Add(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a + b; });
}

ValueResult Sub(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a - b; });
}

ValueResult Mul(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a * b; });
}

ValueResult Divide(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult {
        if (b == 0) {
            return MakeError(ErrKind::kDivZero, "division by zero");
        }
        return a / b;
    });
}

ValueResult Modulo(const ScriptValue& left, const ScriptValue& right) {
    const auto* a = std::get_if<int64_t>(&left);
    const auto* b = std::get_if<int64_t>(&right);
    if (!a || !b) {
        return MakeError(ErrKind::kType, "modulo requires integers");
    }
    if (*b == 0) {
        return MakeError(ErrKind::kDivZero, "modulo by zero");
    }
    return *a % *b;
}

ValueResult Equal(const ScriptValue& left, const ScriptValue& right) {
    return ValuesEqual(left, right);
}

ValueResult NotEqual(const ScriptValue& left, const ScriptValue& right) {
    return !ValuesEqual(left, right);
}

ValueResult Less(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a < b; });
}

ValueResult LessEqual(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a <= b; });
}

ValueResult Greater(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a > b; });
}

ValueResult GreaterEqual(const ScriptValue& left, const ScriptValue& right) {
    return ApplyNumeric(left, right, [](auto a, auto b) -> ValueResult { return a >= b; });
}

template <ValueResult (*Fn)(const ScriptValue&, const ScriptValue&)>
ValueResult StrictRule(const BinOp& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto left, Eval(*node.lhs, ctx));
    VK_ASSIGN_OR_RETURN(auto right, Eval(*node.rhs, ctx));
    return Fn(left, right);
}

template <bool ShortCircuit>
ValueResult LazyRule(const BinOp& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto left, Eval(*node.lhs, ctx));
    if (IsTruthy(left) == ShortCircuit) {
        return ShortCircuit;
    }
    VK_ASSIGN_OR_RETURN(auto right, Eval(*node.rhs, ctx));
    return IsTruthy(right);
}

constexpr DispatchTable<BinaryRule, BinOp::kLast> BuildTable() {
    DispatchTable<BinaryRule, BinOp::kLast> table{};
    table.Set(BinOp::kAdd, &StrictRule<Add>);
    table.Set(BinOp::kSub, &StrictRule<Sub>);
    table.Set(BinOp::kMul, &StrictRule<Mul>);
    table.Set(BinOp::kDiv, &StrictRule<Divide>);
    table.Set(BinOp::kMod, &StrictRule<Modulo>);
    table.Set(BinOp::kEq, &StrictRule<Equal>);
    table.Set(BinOp::kNeq, &StrictRule<NotEqual>);
    table.Set(BinOp::kLt, &StrictRule<Less>);
    table.Set(BinOp::kLe, &StrictRule<LessEqual>);
    table.Set(BinOp::kGt, &StrictRule<Greater>);
    table.Set(BinOp::kGe, &StrictRule<GreaterEqual>);
    table.Set(BinOp::kAnd, &LazyRule<false>);
    table.Set(BinOp::kOr, &LazyRule<true>);
    return table;
}

constexpr auto kBinaryTable = BuildTable();
static_assert(kBinaryTable.Complete(), "BinOp evaluator table incomplete");

}  // namespace bin_detail

ValueResult Eval(const BinOp& node, EvalContext& ctx) {
    return bin_detail::kBinaryTable[node.op](node, ctx);
}

}  // namespace valkey::script
