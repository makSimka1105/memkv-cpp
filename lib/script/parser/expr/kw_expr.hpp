#pragma once

#include <variant>

#include "script/ast.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct KwExprParser {
    static bool Matches(const Token& tok) {
        const auto* kw = std::get_if<TkKw>(&tok);
        return kw && (kw->kw == TkKw::kTrue || kw->kw == TkKw::kFalse || kw->kw == TkKw::kNil);
    }
    template <typename Ep>
    static ParseResult<Expr> Parse(TokenCursor& cursor, Ep&) {
        auto keyword = std::get<TkKw>(cursor.Advance()).kw;
        if (keyword == TkKw::kNil) {
            return NilLit{};
        }
        return BoolLit{keyword == TkKw::kTrue};
    }
};

}  // namespace valkey::script
