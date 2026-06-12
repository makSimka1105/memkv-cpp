#pragma once

#include <optional>
#include <string>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "lex/quoted.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct StringRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        auto scanned = lex::ScanQuoted(c);
        if (!scanned) {
            return std::nullopt;
        }
        if (!scanned->has_value()) {
            if (scanned->error().kind == lex::ScanErrorKind::kBadEscape) {
                return MakeError(ErrKind::kLex,
                                 std::string("unknown escape: \\") + scanned->error().escape);
            }
            return MakeError(ErrKind::kLex, "unterminated string literal");
        }
        return Token{TkStr{std::move(scanned->value())}};
    }
};

}  // namespace valkey::script
