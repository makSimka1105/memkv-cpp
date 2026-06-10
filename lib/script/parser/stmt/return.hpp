#pragma once

#include <optional>
#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct ReturnParser {
    static bool Matches(const Token& tok) { return TokenIsKw(tok, TkKw::kReturn); }
    template <typename Sp>
    static ParseResult<Stmt> Parse(TokenCursor& cursor, Sp& stmt_parser) {
        cursor.Advance();
        if (cursor.Is<TkSemi>()) {
            cursor.Advance();
            return ReturnStmt{std::nullopt};
        }
        VK_ASSIGN_OR_RETURN(auto value, stmt_parser.ParseExpr());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkSemi>("expected ';'"));
        return ReturnStmt{std::move(value)};
    }
};

}  // namespace valkey::script
