#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "lexer/types.hpp"

namespace valkey::parser::detail {

inline bool IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline size_t WordEndPos(std::string_view input, size_t pos) {
    size_t end = pos;
    while (end < input.size() && !IsWhitespace(input[end]) && input[end] != '"') {
        ++end;
    }
    return end;
}

inline std::expected<char, LexError> ProcessEscape(std::string_view input, size_t pos) {
    switch (input[pos]) {
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
            return std::unexpected(LexError{"invalid escape", pos - 1});
    }
}

}  // namespace valkey::parser::detail
