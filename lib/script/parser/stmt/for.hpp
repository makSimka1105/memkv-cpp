#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct ForParser {
    static bool Matches(const Token& tok) { return TokenIsKw(tok, TkKw::kFor); }
    template <typename Sp>
    static ParseResult<Stmt> Parse(ScriptTokenCursor& cursor, Sp& stmt_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto variable,
                            cursor.ExpectT<TkIdent>("expected identifier after 'for'"));
        VK_RETURN_IF_ERROR(cursor.ExpectKw(TkKw::kIn, "expected 'in'"));
        VK_ASSIGN_OR_RETURN(auto iterable, stmt_parser.ParseExpr());
        VK_ASSIGN_OR_RETURN(auto body, stmt_parser.ParseBlock());
        return ForStmt{std::move(variable.name), std::move(iterable), Box<Stmt>{std::move(body)}};
    }
};

}  // namespace valkey::script
