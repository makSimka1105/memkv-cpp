#pragma once
#include <cstddef>
#include <expected>
#include <string_view>
#include "meta/fixed_string.hpp"
#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "schema/parsers/scan.hpp"
#include "util/str.hpp"
namespace valkey::schema {
template <FixedString Kw, class Type>
struct Keyword {
    using Output = Type;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 2;
    static std::expected<Type, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (cursor.MatchKeyword(Kw)) {
            return ScanValue<Type>(cursor);
        }
        return Type{};
    }
    static constexpr bool MatchesKeyword(std::string_view word) { return CaseEqual(word, Kw); }
    static Type Default() { return Type{}; }
};
}  // namespace valkey::schema
