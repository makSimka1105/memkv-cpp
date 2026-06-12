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

        size_t from = c.Pos();
        auto sp = c.Save();
        auto digits = [](char ch) { return std::isdigit(static_cast<unsigned char>(ch)) != 0; };

        c.TakeWhile(digits);
        bool is_double = false;
        if (c.Peek() == '.') {
            is_double = true;
            c.Advance();
            c.TakeWhile(digits);
        }
        if (c.Peek() == 'e' || c.Peek() == 'E') {
            is_double = true;
            c.Advance();
            if (c.Peek() == '+' || c.Peek() == '-') {
                c.Advance();
            }
            c.TakeWhile(digits);
        }
        if (!is_double) {
            return std::nullopt;
        }

        char next = c.Peek();
        if (std::isalpha(static_cast<unsigned char>(next)) || next == '_' || next == '.') {
            return MakeError(ErrKind::kLex, "malformed double literal");
        }

        double val{};
        auto sv = c.SliceFrom(from);
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
            return MakeError(ErrKind::kLex, "malformed double literal");
        }
        sp.Commit();
        return Token{TkDouble{val}};
    }
};

}  // namespace valkey::script
