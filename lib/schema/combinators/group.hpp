#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "meta/fixed_string.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"
#include "schema/combinators/apply_fields.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <FixedString Kw, class Node, FieldFor<Node>... Fields>
struct Group {
    using Output = Node;
    static constexpr size_t kMinArgc = SaturatingAdd(1, SaturatingSum(Fields::kMinArgc...));
    static constexpr size_t kMaxArgc = SaturatingAdd(1, SaturatingSum(Fields::kMaxArgc...));

    static std::expected<Node, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (!cursor.MatchKeyword(Kw)) {
            return std::unexpected(parser::ParseError{"ERR syntax error", cursor.Pos()});
        }
        Node node{};
        auto result = ApplyFields<Fields...>(cursor, node);
        if (!result) {
            return std::unexpected(result.error());
        }
        return node;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return CaseEqual(word, Kw);
    }

    static Node Default() { return Node{}; }
};

template <FixedString Kw, class Node, class... Fields>
using Case = Group<Kw, Node, Fields...>;

}  // namespace valkey::schema
