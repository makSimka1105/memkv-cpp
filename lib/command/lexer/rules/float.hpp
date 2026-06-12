#pragma once

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "command/lexer/types.hpp"
#include "command/lexer/util.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct FloatRule {
    static lex::Match<Token, DiagnosticMessage> Match(cursor::CharCursor& c) {
        std::string_view word = c.PeekWhile(detail::IsWordChar);
        if (word.empty()) {
            return std::nullopt;
        }
        bool has_float_char = false;
        for (char ch : word) {
            if (ch == '.' || ch == 'e' || ch == 'E') {
                has_float_char = true;
                break;
            }
        }
        if (!has_float_char) {
            return std::nullopt;
        }
        double value = 0.0;
        auto res = std::from_chars(word.data(), word.data() + word.size(), value);
        if (res.ec != std::errc{} || res.ptr != word.data() + word.size()) {
            return std::nullopt;
        }
        c.Advance(word.size());
        return Token{TkFloat{value}};
    }
};

}  // namespace valkey::parser
