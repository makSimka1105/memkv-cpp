#pragma once

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

constexpr std::pair<char, Token> kPunct[] = {
    {'(', Token{TkLParen{}}},
    {')', Token{TkRParen{}}},
    {'{', Token{TkLBrace{}}},
    {'}', Token{TkRBrace{}}},
    {'[', Token{TkLBrack{}}},
    {']', Token{TkRBrack{}}},
    {',', Token{TkComma{}}},
    {';', Token{TkSemi{}}},
    {'@', Token{TkAt{}}},
};

struct PunctRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        for (auto& [ch, tok] : kPunct) {
            if (c.Peek() == ch) {
                c.Advance();
                return tok;
            }
        }
        return std::nullopt;
    }
};

}  // namespace valkey::script
