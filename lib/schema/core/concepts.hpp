#pragma once
#include <concepts>
#include <cstddef>
#include <expected>
#include <string_view>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"

namespace valkey::schema {

template <class Type>
concept Parser = requires(parser::Cursor cursor) {
    typename Type::Output;
    {
        Type::Parse(cursor)
    } -> std::same_as<parser::CmdResult<typename Type::Output>>;
    { Type::kMinArgc } -> std::convertible_to<size_t>;
    { Type::kMaxArgc } -> std::convertible_to<size_t>;
};

template <class Type>
concept LabeledParser = Parser<Type> && requires(std::string_view keyword) {
    { Type::MatchesKeyword(keyword) } -> std::convertible_to<bool>;
    { Type::Default() } -> std::same_as<typename Type::Output>;
};

template <class Type, class Node>
concept FieldFor = requires(parser::Cursor cursor, Node& node) {
    { Type::Apply(cursor, node) } -> std::same_as<parser::CmdResult<void>>;
    { Type::kMinArgc } -> std::convertible_to<size_t>;
    { Type::kMaxArgc } -> std::convertible_to<size_t>;
};

template <class Type, class Node>
concept LabeledField = FieldFor<Type, Node> && requires(std::string_view keyword, Node& node) {
    { Type::MatchesKeyword(keyword) } -> std::convertible_to<bool>;
    Type::ApplyDefault(node);
};

}  // namespace valkey::schema
