#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

class TokenCursor {
public:
    explicit TokenCursor(std::vector<SpannedToken> tokens) : tokens_(std::move(tokens)) {}

    const Token& Peek(size_t offset = 0) const {
        size_t i = pos_ + offset;
        return i < tokens_.size() ? tokens_[i].tok : tokens_.back().tok;
    }

    Token Advance() {
        Token t = tokens_[pos_].tok;
        if (pos_ + 1 < tokens_.size()) {
            ++pos_;
        }
        return t;
    }

    template <typename T>
    bool Is(size_t offset = 0) const {
        return std::holds_alternative<T>(Peek(offset));
    }

    template <typename T>
    const T* As(size_t offset = 0) const {
        return std::get_if<T>(&Peek(offset));
    }

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

private:
    std::vector<SpannedToken> tokens_;
    size_t pos_ = 0;
};

}  // namespace valkey::script
