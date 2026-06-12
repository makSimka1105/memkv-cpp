#pragma once

#include <optional>
#include <string>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"

namespace valkey::lex {

constexpr std::optional<char> DecodeEscape(char c) {
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '"':
            return '"';
        case '\\':
            return '\\';
        default:
            return std::nullopt;
    }
}

enum class ScanErrorKind {
    kUnterminated,
    kBadEscape,
};

struct ScanStringError {
    ScanErrorKind kind;
    char escape = '\0';
};

Match<std::string, ScanStringError> ScanQuoted(cursor::CharCursor& c) {
    if (c.Peek() != '"') {
        return std::nullopt;
    }
    c.Advance();

    std::string text;
    while (!c.AtEnd() && c.Peek() != '"') {
        if (c.Peek() == '\\' && c.At(1)) {
            c.Advance();
            auto esc = DecodeEscape(c.Peek());
            if (!esc) {
                return std::unexpected(ScanStringError{ScanErrorKind::kBadEscape, c.Peek()});
            }
            text.push_back(*esc);
            c.Advance();
        } else {
            text.push_back(c.Peek());
            c.Advance();
        }
    }

    if (c.AtEnd()) {
        return std::unexpected(ScanStringError{ScanErrorKind::kUnterminated});
    }
    c.Advance();
    return std::expected<std::string, ScanStringError>{std::move(text)};
}

}  // namespace valkey::lex
