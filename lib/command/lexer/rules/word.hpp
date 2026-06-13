#pragma once

#include <optional>
#include <string_view>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "command/lexer/types.hpp"
#include "command/lexer/util.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct WordRule {
    static lex::Match<Token, DiagnosticMessage> Match(cursor::CharCursor& c) {
        std::string_view word = c.PeekWhile(detail::IsWordChar);
        if (word.empty()) {
            return std::nullopt;
        }
        c.Advance(word.size());
        return Token{TkWord{word}};
    }
};

}  // namespace valkey::parser
