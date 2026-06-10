#pragma once

#include <utility>
#include <vector>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/parser/registry.hpp"
#include "script/token.hpp"

namespace valkey::script {

class Parser {
public:
    explicit Parser(std::vector<SpannedToken> tokens) : tokens_(std::move(tokens)) {}

    ParseResult<Program> ParseProgram() {
        TokenCursor cursor{std::move(tokens_)};
        ActiveExprParser expr_parser{cursor};
        ActiveStmtParser stmt_parser{cursor, expr_parser};
        std::vector<Stmt> stmts;
        while (!cursor.Is<TkEof>()) {
            VK_ASSIGN_OR_RETURN(auto stmt, stmt_parser.ParseStmt());
            stmts.push_back(std::move(stmt));
        }
        return Program{std::move(stmts)};
    }

private:
    std::vector<SpannedToken> tokens_;
};

}  // namespace valkey::script
