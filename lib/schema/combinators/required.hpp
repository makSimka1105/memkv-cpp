#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class Field>
struct Required {
    static constexpr size_t kMinArgc = Field::kMinArgc;
    static constexpr size_t kMaxArgc = Field::kMaxArgc;
    static constexpr bool kRequired = true;

    template <class Node>
        requires LabeledField<Field, Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& cursor, Node& node) {
        return Field::Apply(cursor, node);
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return Field::MatchesKeyword(word);
    }

    template <class Node>
    static void ApplyDefault(Node& node) {
        Field::ApplyDefault(node);
    }
};

template <class Field>
consteval bool FieldRequired() {
    if constexpr (requires { Field::kRequired; }) {
        return Field::kRequired;
    } else {
        return false;
    }
}

}  // namespace valkey::schema
