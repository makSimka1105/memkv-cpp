#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>

#include "lexer/types.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"
#include "util/str.hpp"

namespace valkey::parser {

struct Cursor {
    std::span<const Token> tokens;
    size_t pos = 0;

    bool Done() const { return pos >= tokens.size(); }
    size_t Pos() const { return pos; }
    size_t Remaining() const { return tokens.size() > pos ? tokens.size() - pos : 0; }

    std::expected<std::string, ParseError> NextString() {
        if (Done()) {
            return std::unexpected(ParseError{"unexpected end of input", pos});
        }
        auto r = TokenToString(tokens[pos], pos);
        if (r) {
            ++pos;
        }
        return r;
    }

    std::expected<int64_t, ParseError> NextInt() {
        if (Done()) {
            return std::unexpected(ParseError{"unexpected end of input", pos});
        }
        auto r = TokenToInt(tokens[pos], pos);
        if (r) {
            ++pos;
        }
        return r;
    }

    std::expected<double, ParseError> NextFloat() {
        if (Done()) {
            return std::unexpected(ParseError{"unexpected end of input", pos});
        }
        auto r = TokenToFloat(tokens[pos], pos);
        if (r) {
            ++pos;
        }
        return r;
    }

    std::expected<std::string, ParseError> PeekString() const {
        if (Done()) {
            return std::unexpected(ParseError{"unexpected end of input", pos});
        }
        return TokenToString(tokens[pos], pos);
    }

    bool MatchKeyword(std::string_view kw) {
        if (Done()) {
            return false;
        }
        auto s = TokenToString(tokens[pos], pos);
        if (!s || !CaseEqual(*s, kw)) {
            return false;
        }
        ++pos;
        return true;
    }
};

}  // namespace valkey::parser
