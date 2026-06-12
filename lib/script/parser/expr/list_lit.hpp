#pragma once

#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct ListLitParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkLBrack>(tok); }
    template <typename Ep>
    static ParseResult<Expr> Parse(ScriptTokenCursor& cursor, Ep& expr_parser) {
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto items, expr_parser.template ParseSeparated<TkRBrack>());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkRBrack>("expected ']'"));
        return ListLit{std::move(items)};
    }
};

}  // namespace valkey::script
