#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/error.hpp"
#include "script/parser/core/optables.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"
#include "script/token.hpp"

namespace valkey::script {

template <Dispatchable... Ts>
class ExprParser {
public:
    explicit ExprParser(TokenCursor& cursor) : cursor_(cursor) {}

    ParseResult<Expr> ParsePrimary() {
        std::optional<ParseResult<Expr>> hit;
        ((hit = Try<Ts>()) || ...);
        if (hit) {
            return std::move(*hit);
        }
        return MakeError(ErrKind::kParse, "unexpected token in expression");
    }

    ParseResult<Expr> ParseExpr(int min_binding_power = 0) {
        VK_ASSIGN_OR_RETURN(auto left, ParsePrimary());
        while (true) {
            if (cursor_.Is<TkLBrack>()) {
                if (15 <= min_binding_power) {
                    break;
                }
                cursor_.Advance();
                VK_ASSIGN_OR_RETURN(auto key, ParseExpr());
                VK_RETURN_IF_ERROR(cursor_.ExpectT<TkRBrack>("expected ']'"));
                left = Index{Box<Expr>{std::move(left)}, Box<Expr>{std::move(key)}};
                continue;
            }
            auto op_info = GetOpInfo(cursor_.Peek());
            if (!op_info || op_info->left_bp <= min_binding_power) {
                break;
            }
            cursor_.Advance();
            VK_ASSIGN_OR_RETURN(auto right, ParseExpr(op_info->right_bp));
            left = BinOp{op_info->ast_op, Box<Expr>{std::move(left)}, Box<Expr>{std::move(right)}};
        }
        return left;
    }

    template <typename TClose>
    ParseResult<std::vector<Expr>> ParseSeparated() {
        std::vector<Expr> items;
        while (!cursor_.Is<TClose>() && !cursor_.Is<TkEof>()) {
            VK_ASSIGN_OR_RETURN(auto item, ParseExpr());
            items.push_back(std::move(item));
            if (!cursor_.Is<TClose>()) {
                VK_RETURN_IF_ERROR(cursor_.ExpectT<TkComma>("expected ','"));
            }
        }
        return items;
    }

private:
    template <typename T>
    std::optional<ParseResult<Expr>> Try() {
        if (!T::Matches(cursor_.Peek())) {
            return std::nullopt;
        }
        return T::Parse(cursor_, *this);
    }

    TokenCursor& cursor_;
};

}  // namespace valkey::script
