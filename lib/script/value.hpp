#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "script/box.hpp"
#include "script/error.hpp"

namespace valkey::script {

struct Nil {
    bool operator==(const Nil&) const { return true; }
};

struct List;

using ScriptValue = std::variant<Nil, bool, int64_t, double, std::string, Box<List>>;

struct List {
    std::vector<ScriptValue> items;
    bool operator==(const List& other) const { return items == other.items; }
};

using ValueResult = std::expected<ScriptValue, ScriptError>;

bool IsTruthy(const ScriptValue& value) {
    if (std::holds_alternative<Nil>(value))
        return false;
    if (const bool* flag = std::get_if<bool>(&value))
        return *flag;
    return true;
}

std::string_view TypeName(const ScriptValue& value) {
    switch (value.index()) {
        case 0:
            return "nil";
        case 1:
            return "boolean";
        case 2:
            return "integer";
        case 3:
            return "double";
        case 4:
            return "string";
        default:
            return "list";
    }
}

template <typename T>
concept Numeric = std::same_as<T, int64_t> || std::same_as<T, double>;

using NumericVal = std::variant<int64_t, double>;

template <typename T>
struct NumericRank;
template <>
struct NumericRank<int64_t> : std::integral_constant<int, 0> {};
template <>
struct NumericRank<double> : std::integral_constant<int, 1> {};

template <Numeric L, Numeric R>
using CommonNumeric = std::conditional_t<(NumericRank<L>::value >= NumericRank<R>::value), L, R>;

std::optional<NumericVal> AsNumeric(const ScriptValue& v) {
    if (const auto* i = std::get_if<int64_t>(&v))
        return NumericVal{*i};
    if (const auto* d = std::get_if<double>(&v))
        return NumericVal{*d};
    return std::nullopt;
}

bool ValuesEqual(const ScriptValue& l, const ScriptValue& r) {
    auto nl = AsNumeric(l);
    auto nr = AsNumeric(r);
    if (nl && nr) {
        return std::visit(
            [](auto lv, auto rv) {
                using C = CommonNumeric<std::decay_t<decltype(lv)>, std::decay_t<decltype(rv)>>;
                return static_cast<C>(lv) == static_cast<C>(rv);
            },
            *nl, *nr);
    }
    return l == r;
}


ValueResult ApplyNumeric(const ScriptValue& l, const ScriptValue& r, auto op) {
    auto nl = AsNumeric(l);
    auto nr = AsNumeric(r);
    if (!nl || !nr)
        return MakeError(ErrKind::kType, "operator expects numbers");
    return std::visit(
        [&op](auto lv, auto rv) -> ValueResult {
            using C = CommonNumeric<std::decay_t<decltype(lv)>, std::decay_t<decltype(rv)>>;
            return op(static_cast<C>(lv), static_cast<C>(rv));
        },
        *nl, *nr);
}

}  // namespace valkey::script
