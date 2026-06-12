#pragma once

#include <optional>
#include <string>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct StringRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        if (c.Peek() != '"') {
            return std::nullopt;
        }
        c.Advance();

        std::string value;
        while (!c.AtEnd() && c.Peek() != '"') {
            if (c.Peek() == '\\') {
                c.Advance();
                switch (c.Peek()) {
                    case '"':  value += '"';  break;
                    case '\\': value += '\\'; break;
                    case 'n':  value += '\n'; break;
                    default:
                        return MakeError(ErrKind::kLex,
                                         std::string("unknown escape: \\") + c.Peek());
                }
                c.Advance();
            } else {
                value += c.Peek();
                c.Advance();
            }
        }

        if (c.AtEnd()) {
            return MakeError(ErrKind::kLex, "unterminated string literal");
        }
        c.Advance();
        return Token{TkStr{std::move(value)}};
    }
};

}  // namespace valkey::script
