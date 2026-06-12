#pragma once

#include <cctype>
#include <expected>
#include <string>
#include <vector>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"
#include "script/lexer/rules/double.hpp"
#include "script/lexer/rules/int.hpp"
#include "script/lexer/rules/string.hpp"
#include "script/lexer/rules/kw.hpp"
#include "script/lexer/rules/ident.hpp"
#include "script/lexer/rules/op.hpp"
#include "script/lexer/rules/punct.hpp"

namespace valkey::script {

struct ScriptSpec {
    using Cursor = cursor::CharCursor;
    using Token = script::Token;
    using Error = ScriptError;
    using Rules = lex::RuleSet<DoubleRule, IntRule, StringRule, KwRule, IdentRule, OpRule, PunctRule>;

    static bool Skip(char c) { return std::isspace(static_cast<unsigned char>(c)); }

    static Error OnUnexpected(char c) {
        return ScriptError{ErrKind::kLex, std::string("unexpected character: ") + c};
    }

    static Token Eof() { return TkEof{}; }
};

std::expected<std::vector<Token>, ScriptError> Tokenize(std::string_view src) {
    return lex::Lexer<ScriptSpec>::Run(src);
}

}  // namespace valkey::script
