#pragma once

#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

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
    static MatchResult Match(Cursor& c) {
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
