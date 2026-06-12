#pragma once

#include <optional>
#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct IfParser {
    static bool Matches(const Token& tok) { return TokenIsKw(tok, TkKw::kIf); }
    template <typename Sp>
    static ParseResult<Stmt> Parse(ScriptTokenCursor& cursor, Sp& stmt_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto condition, stmt_parser.ParseExpr());
        VK_ASSIGN_OR_RETURN(auto then_block, stmt_parser.ParseBlock());
        std::optional<Box<Stmt>> else_branch;
        if (cursor.IsKw(TkKw::kElse)) {
            cursor.Advance();
            VK_ASSIGN_OR_RETURN(auto else_block, stmt_parser.ParseBlock());
            else_branch = Box<Stmt>{std::move(else_block)};
        }
        return IfStmt{std::move(condition), Box<Stmt>{std::move(then_block)},
                      std::move(else_branch)};
    }
};

}  // namespace valkey::script
