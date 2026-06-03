#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "lexer/types.hpp"
#include "parser/errors.hpp"
#include "parser/token_convert.hpp"
#include "util/str.hpp"

namespace valkey::parser {

class Cursor {
public:
    explicit Cursor(std::span<const Token> tokens, size_t pos = 0)
        : tokens_(tokens), pos_(pos) {}

    bool Done() const { return pos_ >= tokens_.size(); }
    size_t Pos() const { return pos_; }
    size_t Remaining() const { return tokens_.size() > pos_ ? tokens_.size() - pos_ : 0; }
    void Restore(size_t pos) { pos_ = pos; }

    std::expected<std::string, ParseError> NextString() { return Next<std::string>(); }
    std::expected<int64_t, ParseError> NextInt() { return Next<int64_t>(); }
    std::expected<double, ParseError> NextFloat() { return Next<double>(); }

    std::expected<std::string_view, ParseError> PeekStringView() const {
        if (Done()) {
            return err::Message("unexpected end of input", pos_);
        }
        return TokenToStringView(tokens_[pos_], pos_);
    }

    bool MatchKeyword(std::string_view kw) {
        if (Done()) {
            return false;
        }
        auto v = TokenToStringView(tokens_[pos_], pos_);
        if (!v || !CaseEqual(*v, kw)) {
            return false;
        }
        ++pos_;
        return true;
    }

private:
    template <class T>
    std::expected<T, ParseError> Next() {
        if (Done()) {
            return err::Message("unexpected end of input", pos_);
        }
        auto r = Convert<T>(tokens_[pos_], pos_);
        if (r) {
            ++pos_;
        }
        return r;
    }

    template <class T>
    static std::expected<T, ParseError> Convert(const Token& tok, size_t idx) {
        if constexpr (std::is_same_v<T, std::string>) {
            return TokenToString(tok, idx);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return TokenToInt(tok, idx);
        } else {
            return TokenToFloat(tok, idx);
        }
    }

    std::span<const Token> tokens_;
    size_t pos_ = 0;
};

}  // namespace valkey::parser
