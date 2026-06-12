#pragma once

#include <optional>
#include <string>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "command/lexer/types.hpp"
#include "command/lexer/util.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct QuotedStringRule {
    static lex::Match<Token, DiagnosticMessage> Match(cursor::CharCursor& c) {
        if (c.Peek() != '"') {
            return std::nullopt;
        }
        c.Advance();

        std::string text;
        while (!c.AtEnd() && c.Peek() != '"') {
            if (c.Peek() == '\\' && c.At(1)) {
                c.Advance();
                auto esc = detail::ProcessEscape(c.Peek());
                if (!esc) {
                    return std::unexpected(esc.error());
                }
                text.push_back(*esc);
                c.Advance();
            } else {
                text.push_back(c.Peek());
                c.Advance();
            }
        }

        if (c.AtEnd()) {
            return std::unexpected(DiagnosticMessage{"unterminated string"});
        }
        c.Advance();
        return Token{TkStr{std::move(text)}};
    }
};

}  // namespace valkey::parser
