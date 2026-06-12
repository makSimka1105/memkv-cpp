#pragma once

#include <string>
#include <utility>
#include <vector>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/parser/core/dispatch.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

template <typename ExprParser, Dispatchable... Ts>
class StmtParser {
public:
    StmtParser(ScriptTokenCursor& cursor, ExprParser& expr_parser)
        : cursor_(cursor), expr_parser_(expr_parser) {}

    ParseResult<Expr> ParseExpr(int min_binding_power = 0) {
        return expr_parser_.ParseExpr(min_binding_power);
    }

    ParseResult<Stmt> ParseStmt() {
        auto hit = ParseletSet<Ts...>::template TryParse<Stmt>(cursor_, *this);
        if (hit) {
            return std::move(*hit);
        }
        return ParseExprOrAssign();
    }

    ParseResult<Stmt> ParseBlock() {
        VK_RETURN_IF_ERROR(cursor_.ExpectT<TkLBrace>("expected '{'"));
        std::vector<Stmt> stmts;
        while (!cursor_.Is<TkRBrace>() && !cursor_.Is<TkEof>()) {
            VK_ASSIGN_OR_RETURN(auto stmt, ParseStmt());
            stmts.push_back(std::move(stmt));
        }
        VK_RETURN_IF_ERROR(cursor_.ExpectT<TkRBrace>("expected '}'"));
        return Block{std::move(stmts)};
    }

private:
    ParseResult<Stmt> ParseExprOrAssign() {
        VK_ASSIGN_OR_RETURN(auto expr, ParseExpr());
        if (cursor_.IsOp(TkOp::kAssign)) {
            cursor_.Advance();
            if (!std::holds_alternative<Ident>(expr)) {
                return MakeError(ErrKind::kParse, "left side of '=' must be an identifier");
            }
            std::string name = std::move(std::get<Ident>(expr).name);
            VK_ASSIGN_OR_RETURN(auto value, ParseExpr());
            VK_RETURN_IF_ERROR(cursor_.ExpectT<TkSemi>("expected ';'"));
            return AssignStmt{std::move(name), std::move(value)};
        }
        VK_RETURN_IF_ERROR(cursor_.ExpectT<TkSemi>("expected ';'"));
        return ExprStmt{std::move(expr)};
    }

    ScriptTokenCursor& cursor_;
    ExprParser& expr_parser_;
};

}  // namespace valkey::script
