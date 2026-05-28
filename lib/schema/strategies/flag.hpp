#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "schema/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"

namespace valkey::schema {

template <FixedString Kw>
struct Flag {
    using Output = bool;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<bool, parser::ParseError> Parse(parser::Cursor& c) {
        return c.MatchKeyword(Kw);
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return parser::CaseEqual(word, Kw);
    }

    static constexpr bool Default() { return false; }
};

}  // namespace valkey::schema
