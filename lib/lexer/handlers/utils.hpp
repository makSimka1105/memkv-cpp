#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "diagnostic.hpp"
#include "lexer/types.hpp"

namespace valkey::parser::detail {

bool IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

size_t WordEndPos(std::string_view input, size_t pos) {
    size_t end = pos;
    while (end < input.size() && !IsWhitespace(input[end]) && input[end] != '"') {
        ++end;
    }
    return end;
}

std::expected<char, DiagnosticMessage> ProcessEscape(std::string_view input, size_t pos) {
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
            return std::unexpected(DiagnosticMessage{"invalid escape"});
    }
}

}  // namespace valkey::parser::detail
