#pragma once

#include <expected>

#include "command/parser/cursor.hpp"
#include "command/schema/core/concepts.hpp"
#include "command/diagnostic.hpp"

namespace valkey::schema {

template <class... Fields, class Node>
    requires(FieldFor<Fields, Node> && ...)
parser::CmdResult<void> ApplyFields(parser::CmdTokenCursor& cursor, Node& node) {
    parser::CmdResult<void> result{};
    static_cast<void>(((result = Fields::Apply(cursor, node)).has_value() && ...));
    return result;
}

}  // namespace valkey::schema
