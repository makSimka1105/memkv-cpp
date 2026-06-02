#pragma once
#include <cstddef>
#include <expected>
#include <string_view>
#include "meta/fixed_string.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "util/str.hpp"
namespace valkey::schema {
template <FixedString Kw>
struct Flag {
    using Output = bool;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;
    static std::expected<bool, parser::ParseError> Parse(parser::Cursor& cursor) { return cursor.MatchKeyword(Kw); }
    static constexpr bool MatchesKeyword(std::string_view word) { return CaseEqual(word, Kw); }
    static constexpr bool Default() { return false; }
};
}  // namespace valkey::schema
