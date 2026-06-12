#pragma once

#include <cctype>
#include <string_view>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

constexpr std::pair<std::string_view, TkKw::Keyword> kKeywords[] = {
    {"let",    TkKw::kLet},
    {"if",     TkKw::kIf},
    {"else",   TkKw::kElse},
    {"while",  TkKw::kWhile},
    {"for",    TkKw::kFor},
    {"in",     TkKw::kIn},
    {"return", TkKw::kReturn},
    {"and",    TkKw::kAnd},
    {"or",     TkKw::kOr},
    {"not",    TkKw::kNot},
    {"nil",    TkKw::kNil},
    {"true",   TkKw::kTrue},
    {"false",  TkKw::kFalse},
};

struct KwRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        if (!std::isalpha(static_cast<unsigned char>(c.Peek())) && c.Peek() != '_') {
            return std::nullopt;
        }

        for (auto& [str, kw] : kKeywords) {
            if (c.StartsWith(str)) {
                char after = c.Peek(str.size());
                if (!std::isalnum(static_cast<unsigned char>(after)) && after != '_') {
                    c.Advance(str.size());
                    return Token{TkKw{kw}};
                }
            }
        }
        return std::nullopt;
    }
};

}  // namespace valkey::script
