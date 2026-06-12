#pragma once

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/parser/core/optables.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct PrefixUnaryParser {
    static bool Matches(const Token& tok) { return GetPrefixUnary(tok).has_value(); }
    template <typename Ep>
    static ParseResult<Expr> Parse(ScriptTokenCursor& cursor, Ep& expr_parser) {
        auto prefix = GetPrefixUnary(cursor.Peek());
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto operand, expr_parser.ParseExpr(prefix->bp));
        return UnOp{prefix->kind, Box<Expr>{std::move(operand)}};
    }
};

}  // namespace valkey::script
