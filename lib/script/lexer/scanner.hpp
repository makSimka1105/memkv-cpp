#pragma once

#include <cctype>
#include <expected>
#include <vector>

#include "script/error.hpp"
#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"
#include "script/lexer/double_rule.hpp"
#include "script/lexer/int_rule.hpp"
#include "script/lexer/string_rule.hpp"
#include "script/lexer/kw_rule.hpp"
#include "script/lexer/ident_rule.hpp"
#include "script/lexer/op_rule.hpp"
#include "script/lexer/punct_rule.hpp"

namespace valkey::script {

using LexChain =
    RuleSet<DoubleRule, IntRule, StringRule, KwRule, IdentRule, OpRule, PunctRule>;

std::expected<std::vector<SpannedToken>, ScriptError> Lex(std::string_view src) {
    Cursor c{src};
    std::vector<SpannedToken> tokens;

    while (!c.AtEnd()) {
        c.TakeWhile([](char ch) { return std::isspace(static_cast<unsigned char>(ch)); });
        if (c.AtEnd()) break;

        auto start = static_cast<uint32_t>(c.pos);
        auto r = LexChain::Try(c);
        if (!r) return std::unexpected(r.error());
        tokens.push_back(SpannedToken{std::move(*r), Span{start, static_cast<uint32_t>(c.pos)}});
    }

    auto eof_pos = static_cast<uint32_t>(c.pos);
    tokens.push_back(SpannedToken{TkEof{}, Span{eof_pos, eof_pos}});
    return tokens;
}

}  // namespace valkey::script
