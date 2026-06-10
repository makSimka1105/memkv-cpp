#pragma once

#include <algorithm>
#include <cstddef>
#include <expected>
#include <string_view>
#include <utility>
#include <variant>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "schema/core/concepts.hpp"
#include "meta/match_first.hpp"

namespace valkey::schema {

template <LabeledParser... Members>
struct OneOf {
    using Type = std::variant<typename Members::Output...>;
    using Output = Type;
    static constexpr size_t kMinArgc = std::min({Members::kMinArgc...});
    static constexpr size_t kMaxArgc = std::max({Members::kMaxArgc...});

    static parser::CmdResult<Output> Parse(parser::Cursor& cursor) {
        auto peeked_word = cursor.PeekStringView();
        if (!peeked_word) {
            return parser::err::SyntaxError();
        }
        std::string_view word = *peeked_word;
        parser::CmdResult<Output> result =
            parser::err::SyntaxError();
        MatchFirst<Members...>([&]<size_t I, class Member>() -> bool {
            if (!Member::MatchesKeyword(word)) {
                return false;
            }
            auto parsed = Member::Parse(cursor);
            if (parsed) {
                result = Output{std::in_place_index<I>, std::move(*parsed)};
            } else {
                result = std::unexpected(parsed.error());
            }
            return true;
        });
        return result;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return (Members::MatchesKeyword(word) || ...);
    }

    static Output Default() { return Output{}; }
};

}  // namespace valkey::schema
