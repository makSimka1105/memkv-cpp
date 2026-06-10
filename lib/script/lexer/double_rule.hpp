#pragma once

#include <cctype>
#include <charconv>

#include "script/error.hpp"
#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

namespace valkey::script {

struct DoubleRule {
    static MatchResult Match(Cursor& c) {
        if (!std::isdigit(static_cast<unsigned char>(c.Peek()))) return std::nullopt;

        size_t i = c.pos;
        while (i < c.src.size() && std::isdigit(static_cast<unsigned char>(c.src[i]))) ++i;
        if (i >= c.src.size() || c.src[i] != '.') return std::nullopt;

        size_t from = c.pos;
        c.TakeWhile([](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });
        c.Advance();
        c.TakeWhile([](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });

        double val{};
        auto sv = c.SliceFrom(from);
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec != std::errc{}) return MakeError(ErrKind::kLex, "malformed double literal");
        return Token{TkDouble{val}};
    }
};

}  // namespace valkey::script
