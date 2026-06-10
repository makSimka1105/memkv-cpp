#pragma once

#include <array>
#include <cstdint>
#include <variant>

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

constexpr std::array<BinaryRule, BinOp::kLast> BuildTable() {
    std::array<BinaryRule, BinOp::kLast> table{};
    table[BinOp::kAdd] = &StrictRule<Add>;
    table[BinOp::kSub] = &StrictRule<Sub>;
    table[BinOp::kMul] = &StrictRule<Mul>;
    table[BinOp::kDiv] = &StrictRule<Divide>;
    table[BinOp::kMod] = &StrictRule<Modulo>;
    table[BinOp::kEq] = &StrictRule<Equal>;
    table[BinOp::kNeq] = &StrictRule<NotEqual>;
    table[BinOp::kLt] = &StrictRule<Less>;
    table[BinOp::kLe] = &StrictRule<LessEqual>;
    table[BinOp::kGt] = &StrictRule<Greater>;
    table[BinOp::kGe] = &StrictRule<GreaterEqual>;
    table[BinOp::kAnd] = &LazyRule<false>;
    table[BinOp::kOr] = &LazyRule<true>;
    return table;
}

constexpr auto kBinaryTable = BuildTable();

}  // namespace bin_detail

ValueResult Eval(const BinOp& node, EvalContext& ctx) {
    return bin_detail::kBinaryTable[node.op](node, ctx);
}

}  // namespace valkey::script
