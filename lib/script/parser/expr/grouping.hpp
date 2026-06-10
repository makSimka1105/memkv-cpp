#pragma once

#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct GroupingParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkLParen>(tok); }
    template <typename Ep>
    static ParseResult<Expr> Parse(TokenCursor& cursor, Ep& expr_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto inner, expr_parser.ParseExpr());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkRParen>("expected ')'"));
        return inner;
    }
};

}  // namespace valkey::script
