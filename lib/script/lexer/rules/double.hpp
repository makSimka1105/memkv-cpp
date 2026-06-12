#pragma once

#include <cctype>
#include <charconv>
#include <cstddef>
#include <optional>
#include <system_error>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct DoubleRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        if (!std::isdigit(static_cast<unsigned char>(c.Peek()))) {
            return std::nullopt;
        }

        size_t i = 0;
        while (std::isdigit(static_cast<unsigned char>(c.Peek(i)))) {
            ++i;
        }
        if (c.Peek(i) != '.') {
            return std::nullopt;
        }

        size_t from = c.Pos();
        c.TakeWhile([](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });
        c.Advance();
        c.TakeWhile([](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });

        double val{};
        auto sv = c.SliceFrom(from);
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec != std::errc{}) {
            return MakeError(ErrKind::kLex, "malformed double literal");
        }
        return Token{TkDouble{val}};
    }
};

}  // namespace valkey::script
