#pragma once

#include <variant>

#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct IntLitParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkInt>(tok); }
    template <typename Ep>
    static ParseResult<Expr> Parse(ScriptTokenCursor& cursor, Ep&) {
        return IntLit{std::get<TkInt>(cursor.Advance()).value};
    }
};

struct DblLitParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkDouble>(tok); }
    template <typename Ep>
    static ParseResult<Expr> Parse(ScriptTokenCursor& cursor, Ep&) {
        return DblLit{std::get<TkDouble>(cursor.Advance()).value};
    }
};

struct StrLitParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkStr>(tok); }
    template <typename Ep>
    static ParseResult<Expr> Parse(ScriptTokenCursor& cursor, Ep&) {
        return StrLit{std::get<TkStr>(cursor.Advance()).value};
    }
};

}  // namespace valkey::script
