#pragma once

#include <cctype>
#include <string_view>

#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

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
    static MatchResult Match(Cursor& c) {
        if (!std::isalpha(static_cast<unsigned char>(c.Peek())) && c.Peek() != '_')
            return std::nullopt;

        for (auto& [str, kw] : kKeywords) {
            if (c.src.substr(c.pos).starts_with(str)) {
                char after = c.pos + str.size() < c.src.size()
                                 ? c.src[c.pos + str.size()]
                                 : '\0';
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
