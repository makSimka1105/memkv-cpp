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
#include "parser/errors.hpp"
#include "meta/try.hpp"

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
                return err::UnexpectedEof(idx);
            },
            [idx](const TkSkip&) -> std::expected<std::string, ParseError> {
                return err::Message("unexpected skip token", idx);
            },
        },
        tok);
}

std::expected<std::string_view, ParseError> TokenToStringView(const Token& tok, size_t idx) {
    return std::visit(
        Overload{
            [](const TkWord& w) -> std::expected<std::string_view, ParseError> { return w.text; },
            [](const TkStr& s) -> std::expected<std::string_view, ParseError> { return s.value; },
            [idx](const TkEof&) -> std::expected<std::string_view, ParseError> {
                return err::UnexpectedEof(idx);
            },
            [idx](const auto&) -> std::expected<std::string_view, ParseError> {
                return err::Message("unexpected token", idx);
            },
        },
        tok);
}

std::expected<int64_t, ParseError> TokenToInt(const Token& tok, size_t idx) {
    return std::visit(
        Overload{
            [](const TkInt& i) -> std::expected<int64_t, ParseError> { return i.value; },
            [idx](const auto&) -> std::expected<int64_t, ParseError> {
                return err::NotInteger(idx);
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
                return err::NotFloat(idx);
            },
        },
        tok);
}

std::expected<std::vector<std::string>, ParseError> CollectStrings(
    std::span<const Token> args, size_t base_index) {
    std::vector<std::string> out;
    out.reserve(args.size());
    for (size_t i = 0; i < args.size(); ++i) {
        VK_ASSIGN_OR_RETURN(auto s, TokenToString(args[i], base_index + i));
        out.push_back(std::move(s));
    }
    return out;
}

}  // namespace valkey::parser
