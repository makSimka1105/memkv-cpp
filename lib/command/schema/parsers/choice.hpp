#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "command/parser/cursor.hpp"
#include "command/parser/token_convert.hpp"
#include "command/diagnostic.hpp"
#include "meta/fixed_string.hpp"

namespace valkey::schema {

template <FixedString A, FixedString B>
struct Choice {
    using Output = bool;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static parser::CmdResult<bool> Parse(parser::CmdTokenCursor& cursor) {
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
