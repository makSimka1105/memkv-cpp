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

#include "command/lexer/types.hpp"
#include "meta/overload.hpp"
#include "command/diagnostic.hpp"
#include "meta/try.hpp"

namespace valkey::parser {

CmdResult<std::string> TokenToString(const Token& tok, size_t) {
    return std::visit(
        Overload{
            [](const TkWord& w) -> CmdResult<std::string> { return std::string(w.text); },
            [](const TkInt& i) -> CmdResult<std::string> {
                return std::to_string(i.value);
            },
            [](const TkFloat& f) -> CmdResult<std::string> {
                return std::to_string(f.value);
            },
            [](const TkStr& s) -> CmdResult<std::string> { return s.value; },
            [](const TkEof&) -> CmdResult<std::string> {
                return err::UnexpectedEof();
            },
        },
        tok);
}

CmdResult<std::string_view> TokenToStringView(const Token& tok, size_t) {
    return std::visit(
        Overload{
            [](const TkWord& w) -> CmdResult<std::string_view> { return w.text; },
            [](const TkStr& s) -> CmdResult<std::string_view> { return s.value; },
            [](const TkEof&) -> CmdResult<std::string_view> {
                return err::UnexpectedEof();
            },
            [](const auto&) -> CmdResult<std::string_view> {
                return err::Message("unexpected token");
            },
        },
        tok);
}

CmdResult<int64_t> TokenToInt(const Token& tok, size_t) {
    return std::visit(
        Overload{
            [](const TkInt& i) -> CmdResult<int64_t> { return i.value; },
            [](const auto&) -> CmdResult<int64_t> {
                return err::NotInteger();
            },
        },
        tok);
}

CmdResult<double> TokenToFloat(const Token& tok, size_t) {
    return std::visit(
        Overload{
            [](const TkFloat& f) -> CmdResult<double> { return f.value; },
            [](const TkInt& i) -> CmdResult<double> {
                return static_cast<double>(i.value);
            },
            [](const auto&) -> CmdResult<double> {
                return err::NotFloat();
            },
        },
        tok);
}

CmdResult<std::vector<std::string>> CollectStrings(
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
