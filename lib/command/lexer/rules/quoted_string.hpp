#pragma once

#include <optional>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "lex/quoted.hpp"
#include "command/lexer/types.hpp"
#include "command/diagnostic.hpp"

namespace valkey::parser {

struct QuotedStringRule {
    static lex::Match<Token, DiagnosticMessage> Match(cursor::CharCursor& c) {
        auto scanned = lex::ScanQuoted(c);
        if (!scanned) {
            return std::nullopt;
        }
        if (!scanned->has_value()) {
            if (scanned->error().kind == lex::ScanErrorKind::kBadEscape) {
                return std::unexpected(DiagnosticMessage{"invalid escape"});
            }
            return std::unexpected(DiagnosticMessage{"unterminated string"});
        }
        return Token{TkStr{std::move(scanned->value())}};
    }
};

}  // namespace valkey::parser
