#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <string_view>

#include "schema/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"

namespace valkey::schema {

constexpr size_t SaturatingAdd(size_t a, size_t b) {
    if (a == kUnbounded || b == kUnbounded) {
        return kUnbounded;
    }
    if (a > kUnbounded - b) {
        return kUnbounded;
    }
    return a + b;
}

template <class... Values>
constexpr size_t SaturatingSum(Values... values) {
    size_t total = 0;
    ((total = SaturatingAdd(total, values)), ...);
    return total;
}

template <class S>
concept Strategy = requires(parser::Cursor& c) {
    typename S::Output;
    { S::Parse(c) } -> std::same_as<std::expected<typename S::Output, parser::ParseError>>;
    { S::kMinArgc } -> std::convertible_to<size_t>;
    { S::kMaxArgc } -> std::convertible_to<size_t>;
};

template <class R, class Node>
concept RuleFor = requires(parser::Cursor& c, Node& n) {
    { R::Apply(c, n) } -> std::same_as<std::expected<void, parser::ParseError>>;
    { R::kMinArgc } -> std::convertible_to<size_t>;
    { R::kMaxArgc } -> std::convertible_to<size_t>;
};

template <class R, class Node>
concept OptionalRule = RuleFor<R, Node> && requires(Node& n, std::string_view kw) {
    { R::MatchesKeyword(kw) } -> std::convertible_to<bool>;
    R::ApplyDefault(n);
};

}  // namespace valkey::schema
