#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "meta/fixed_string.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"

namespace valkey::schema {

template <FixedString A, FixedString B>
struct Choice {
    using Output = bool;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<bool, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (cursor.MatchKeyword(A)) {
            return true;
        }
        if (cursor.MatchKeyword(B)) {
            return false;
        }
        return false;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return CaseEqual(word, A) || CaseEqual(word, B);
    }

    static constexpr bool Default() { return false; }
};

}  // namespace valkey::schema
