#pragma once

#include <expected>

#include "command/diagnostic.hpp"

namespace valkey::parser::detail {

bool IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool IsWordChar(char c) {
    return !IsWhitespace(c) && c != '"';
}

std::expected<char, DiagnosticMessage> ProcessEscape(char c) {
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
            return std::unexpected(DiagnosticMessage{"invalid escape"});
    }
}

}  // namespace valkey::parser::detail
