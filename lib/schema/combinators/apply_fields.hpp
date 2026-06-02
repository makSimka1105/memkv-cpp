#pragma once

#include <expected>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class... Fields, class Node>
    requires(FieldFor<Fields, Node> && ...)
std::expected<void, parser::ParseError> ApplyFields(parser::Cursor& cursor, Node& node) {
    std::expected<void, parser::ParseError> result{};
    static_cast<void>(((result = Fields::Apply(cursor, node)).has_value() && ...));
    return result;
}

}  // namespace valkey::schema
