#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "command/parser/cursor.hpp"
#include "command/parser/token_convert.hpp"
#include "command/schema/combinators/apply_fields.hpp"
#include "command/schema/core/argc.hpp"
#include "command/schema/core/concepts.hpp"
#include "command/diagnostic.hpp"
#include "meta/fixed_string.hpp"
#include "meta/try.hpp"

namespace valkey::schema {

template <FixedString Kw, class Node, FieldFor<Node>... Fields>
struct Group {
    using Output = Node;
    static constexpr size_t kMinArgc = SaturatingAdd(1, SaturatingSum(Fields::kMinArgc...));
    static constexpr size_t kMaxArgc = SaturatingAdd(1, SaturatingSum(Fields::kMaxArgc...));

    static parser::CmdResult<Node> Parse(parser::CmdTokenCursor& cursor) {
        if (!cursor.MatchKeyword(Kw)) {
            return parser::err::SyntaxError();
        }
        Node node{};
        VK_RETURN_IF_ERROR(ApplyFields<Fields...>(cursor, node));
        return node;
    }

    static constexpr bool MatchesKeyword(std::string_view word) { return CaseEqual(word, Kw); }

    static Node Default() { return Node{}; }
};

template <FixedString Kw, class Node, class... Fields>
using Case = Group<Kw, Node, Fields...>;

}  // namespace valkey::schema
