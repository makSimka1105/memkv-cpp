#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct LetParser {
    static bool Matches(const Token& tok) { return TokenIsKw(tok, TkKw::kLet); }
    template <typename Sp>
    static ParseResult<Stmt> Parse(ScriptTokenCursor& cursor, Sp& stmt_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto name, cursor.ExpectT<TkIdent>("expected identifier after 'let'"));
        VK_RETURN_IF_ERROR(cursor.ExpectOp(TkOp::kAssign, "expected '='"));
        VK_ASSIGN_OR_RETURN(auto initializer, stmt_parser.ParseExpr());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkSemi>("expected ';'"));
        return LetStmt{std::move(name.name), std::move(initializer)};
    }
};

}  // namespace valkey::script
