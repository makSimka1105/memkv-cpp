#pragma once

#include <cctype>

#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

namespace valkey::script {

struct IdentRule {
    static MatchResult Match(Cursor& c) {
        if (!std::isalpha(static_cast<unsigned char>(c.Peek())) && c.Peek() != '_')
            return std::nullopt;

        size_t from = c.pos;
        c.TakeWhile([](char ch) {
            return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
        });
        return Token{TkIdent{std::string(c.SliceFrom(from))}};
    }
};

}  // namespace valkey::script
