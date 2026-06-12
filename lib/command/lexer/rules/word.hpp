#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utility>

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
        std::string text(word);
        c.Advance(word.size());
        return Token{TkWord{std::move(text)}};
    }
};

}  // namespace valkey::parser
