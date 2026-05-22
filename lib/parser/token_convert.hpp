#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "lexer/types.hpp"
#include "meta/overload.hpp"
#include "parser/error.hpp"

namespace valkey::parser {

std::expected<std::string, ParseError> TokenToString(const Token& tok, size_t idx) {
    return std::visit(
        Overload{
            [](const TkWord& w) -> std::expected<std::string, ParseError> { return w.text; },
            [](const TkInt& i) -> std::expected<std::string, ParseError> {
                return std::to_string(i.value);
            },
            [](const TkFloat& f) -> std::expected<std::string, ParseError> {
                return std::to_string(f.value);
            },
            [](const TkStr& s) -> std::expected<std::string, ParseError> { return s.value; },
            [idx](const TkEof&) -> std::expected<std::string, ParseError> {
                return std::unexpected(ParseError{"unexpected EOF", idx});
            },
            [idx](const TkSkip&) -> std::expected<std::string, ParseError> {
                return std::unexpected(ParseError{"unexpected skip token", idx});
            },
        },
        tok);
}

std::expected<int64_t, ParseError> TokenToInt(const Token& tok, size_t idx) {
    return std::visit(
        Overload{
            [](const TkInt& i) -> std::expected<int64_t, ParseError> { return i.value; },
            [idx](const auto&) -> std::expected<int64_t, ParseError> {
                return std::unexpected(
                    ParseError{"ERR value is not an integer or out of range", idx});
            },
        },
        tok);
}

std::expected<double, ParseError> TokenToFloat(const Token& tok, size_t idx) {
    return std::visit(
        Overload{
            [](const TkFloat& f) -> std::expected<double, ParseError> { return f.value; },
            [](const TkInt& i) -> std::expected<double, ParseError> {
                return static_cast<double>(i.value);
            },
            [idx](const auto&) -> std::expected<double, ParseError> {
                return std::unexpected(ParseError{"ERR value is not a valid float", idx});
            },
        },
        tok);
}

std::expected<std::vector<std::string>, ParseError> CollectStrings(
    std::span<const Token> args, size_t base_index) {
    std::vector<std::string> out;
    out.reserve(args.size());
    for (size_t i = 0; i < args.size(); ++i) {
        auto s = TokenToString(args[i], base_index + i);
        if (!s) {
            return std::unexpected(s.error());
        }
        out.push_back(std::move(*s));
    }
    return out;
}

constexpr char ToUpper(char c) {
    return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
}

constexpr bool CaseEqual(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (ToUpper(a[i]) != ToUpper(b[i])) {
            return false;
        }
    }
    return true;
}

}  // namespace valkey::parser
