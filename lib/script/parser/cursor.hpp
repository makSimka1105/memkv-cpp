#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>

#include "cursor/token_cursor.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

class ScriptTokenCursor : public cursor::TokenCursor<Token> {
public:
    using cursor::TokenCursor<Token>::TokenCursor;

    bool IsKw(TkKw::Keyword kw, size_t offset = 0) const {
        const auto* k = As<TkKw>(offset);
        return k && k->kw == kw;
    }

    bool IsOp(TkOp::OpKind op, size_t offset = 0) const {
        const auto* o = As<TkOp>(offset);
        return o && o->op == op;
    }

    template <typename T>
    std::expected<T, ScriptError> ExpectT(std::string_view msg) {
        if (const auto* t = As<T>()) {
            T val = *t;
            Advance();
            return val;
        }
        return MakeError(ErrKind::kParse, std::string(msg));
    }

    std::expected<TkKw, ScriptError> ExpectKw(TkKw::Keyword kw, std::string_view msg) {
        if (IsKw(kw)) {
            TkKw val = *As<TkKw>();
            Advance();
            return val;
        }
        return MakeError(ErrKind::kParse, std::string(msg));
    }

    std::expected<TkOp, ScriptError> ExpectOp(TkOp::OpKind op, std::string_view msg) {
        if (IsOp(op)) {
            TkOp val = *As<TkOp>();
            Advance();
            return val;
        }
        return MakeError(ErrKind::kParse, std::string(msg));
    }
};

}  // namespace valkey::script
