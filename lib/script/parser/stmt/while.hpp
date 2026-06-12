#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct WhileParser {
    static bool Matches(const Token& tok) { return TokenIsKw(tok, TkKw::kWhile); }
    template <typename Sp>
    static ParseResult<Stmt> Parse(ScriptTokenCursor& cursor, Sp& stmt_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto condition, stmt_parser.ParseExpr());
        VK_ASSIGN_OR_RETURN(auto body, stmt_parser.ParseBlock());
        return WhileStmt{std::move(condition), Box<Stmt>{std::move(body)}};
    }
};

}  // namespace valkey::script
