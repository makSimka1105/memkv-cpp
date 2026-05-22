#pragma once

#include <expected>

#include "parser/cursor.hpp"
#include "parser/error.hpp"

namespace valkey::schema {

template <class... Rules, class Node>
std::expected<void, parser::ParseError> ApplyRules(parser::Cursor& c, Node& node) {
    std::expected<void, parser::ParseError> result{};
    static_cast<void>(((result = Rules::Apply(c, node)).has_value() && ...));
    return result;
}

}  // namespace valkey::schema
