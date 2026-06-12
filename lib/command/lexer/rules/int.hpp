#pragma once

#include <charconv>
#include <cstdint>
#include <optional>
#include <string_view>
#include <system_error>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "command/lexer/types.hpp"
#include "command/lexer/util.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct IntRule {
    static lex::Match<Token, DiagnosticMessage> Match(cursor::CharCursor& c) {
        std::string_view word = c.PeekWhile(detail::IsWordChar);
        if (word.empty()) {
            return std::nullopt;
        }
        if (word[0] == '0' && word.size() > 1) {
            return std::nullopt;
        }
        if (word[0] == '-' && word.size() > 2 && word[1] == '0') {
            return std::nullopt;
        }
        int64_t value = 0;
        auto res = std::from_chars(word.data(), word.data() + word.size(), value);
        if (res.ec != std::errc{} || res.ptr != word.data() + word.size()) {
            return std::nullopt;
        }
        c.Advance(word.size());
        return Token{TkInt{value}};
    }
};

}  // namespace valkey::parser
