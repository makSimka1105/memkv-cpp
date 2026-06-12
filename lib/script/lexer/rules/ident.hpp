#pragma once

#include <cctype>
#include <cstddef>
#include <optional>
#include <string>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct IdentRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        if (!std::isalpha(static_cast<unsigned char>(c.Peek())) && c.Peek() != '_') {
            return std::nullopt;
        }

        size_t from = c.Pos();
        c.TakeWhile([](char ch) {
            return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
        });
        return Token{TkIdent{std::string(c.SliceFrom(from))}};
    }
};

}  // namespace valkey::script
