#pragma once

#include <expected>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class... Fields, class Node>
    requires(FieldFor<Fields, Node> && ...)
parser::CmdResult<void> ApplyFields(parser::Cursor& cursor, Node& node) {
    parser::CmdResult<void> result{};
    static_cast<void>(((result = Fields::Apply(cursor, node)).has_value() && ...));
    return result;
}

}  // namespace valkey::schema
