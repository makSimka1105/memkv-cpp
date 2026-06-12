#pragma once

#include <expected>
#include <string_view>
#include <vector>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "command/lexer/rules/float.hpp"
#include "command/lexer/rules/int.hpp"
#include "command/lexer/rules/quoted_string.hpp"
#include "command/lexer/rules/word.hpp"
#include "command/lexer/types.hpp"
#include "command/lexer/util.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct CmdSpec {
    using Cursor = cursor::CharCursor;
    using Token = parser::Token;
    using Error = DiagnosticMessage;
    using Rules = lex::RuleSet<QuotedStringRule, IntRule, FloatRule, WordRule>;

    static bool Skip(char c) { return detail::IsWhitespace(c); }

    static Error OnUnexpected(char) { return DiagnosticMessage{"no handler matched"}; }

    static Token Eof() { return TkEof{}; }
};

std::expected<std::vector<Token>, DiagnosticMessage> Tokenize(std::string_view input) {
    return lex::Lexer<CmdSpec>::Run(input);
}

}  // namespace valkey::parser
