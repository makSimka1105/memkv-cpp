#pragma once

#include <optional>
#include <string>
#include <utility>

#include "script/error.hpp"
#include "script/lexer/cursor.hpp"
#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

namespace valkey::script {

struct StringRule {
    static MatchResult Match(Cursor& c) {
        if (c.Peek() != '"') return std::nullopt;
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

        if (c.AtEnd()) return MakeError(ErrKind::kLex, "unterminated string literal");
        c.Advance();
        return Token{TkStr{std::move(value)}};
    }
};

}  // namespace valkey::script
