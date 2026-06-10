#pragma once

#include <string>
#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct IdentOrCallParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkIdent>(tok); }
    template <typename ExprParser>
    static ParseResult<Expr> Parse(TokenCursor& cursor, ExprParser& expr_parser) {
        std::string name = std::get<TkIdent>(cursor.Advance()).name;
        if (!cursor.Is<TkLParen>()) {
            return Ident{std::move(name)};
        }
        cursor.Advance();
        VK_ASSIGN_OR_RETURN(auto args, expr_parser.template ParseSeparated<TkRParen>());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkRParen>("expected ')'"));
        return Call{std::move(name), std::move(args)};
    }
};

struct ScriptCallParser {
    static bool Matches(const Token& tok) { return std::holds_alternative<TkAt>(tok); }
    template <typename ExprParser>
    static ParseResult<Expr> Parse(TokenCursor& cursor, ExprParser& expr_parser) {
        cursor.Advance();
        const auto* ident = cursor.As<TkIdent>();
        if (!ident) {
            return MakeError(ErrKind::kParse, "expected function name after '@'");
        }
        std::string name = ident->name;
        cursor.Advance();
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkLParen>("expected '(' after '@" + name + "'"));
        VK_ASSIGN_OR_RETURN(auto args, expr_parser.template ParseSeparated<TkRParen>());
        VK_RETURN_IF_ERROR(cursor.ExpectT<TkRParen>("expected ')'"));
        return ScriptCall{std::move(name), std::move(args)};
    }
};

}  // namespace valkey::script
