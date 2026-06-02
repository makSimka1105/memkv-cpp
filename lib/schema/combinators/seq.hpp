#pragma once

#include <cstddef>
#include <expected>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/combinators/apply_fields.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class Node, FieldFor<Node>... Fields>
struct Seq {
    using Output = Node;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = SaturatingSum(Fields::kMaxArgc...);

    static std::expected<Node, parser::ParseError> Parse(parser::Cursor& cursor) {
        Node node{};
        auto result = ApplyFields<Fields...>(cursor, node);
        if (!result) {
            return std::unexpected(result.error());
        }
        return node;
    }
};

}  // namespace valkey::schema
