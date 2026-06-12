#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <type_traits>

#include "command/lexer/types.hpp"
#include "command/parser/token_convert.hpp"
#include "cursor/token_cursor.hpp"
#include "command/diagnostic.hpp"
#include "util/str.hpp"

namespace valkey::parser {

class CmdTokenCursor : public cursor::TokenCursor<Token> {
public:
    using cursor::TokenCursor<Token>::TokenCursor;

    CmdResult<std::string> NextString() { return Next<std::string>(); }
    CmdResult<int64_t> NextInt() { return Next<int64_t>(); }
    CmdResult<double> NextFloat() { return Next<double>(); }

    CmdResult<std::string_view> PeekStringView() const {
        const Token* tok = At();
        if (!tok) {
            return err::Message("unexpected end of input");
        }
        return TokenToStringView(*tok, Pos());
    }

    bool MatchKeyword(std::string_view kw) {
        const Token* tok = At();
        if (!tok) {
            return false;
        }
        auto v = TokenToStringView(*tok, Pos());
        if (!v || !CaseEqual(*v, kw)) {
            return false;
        }
        Advance();
        return true;
    }

private:
    template <class T>
    CmdResult<T> Next() {
        const Token* tok = At();
        if (!tok) {
            return err::Message("unexpected end of input");
        }
        auto r = Convert<T>(*tok, Pos());
        if (r) {
            Advance();
        }
        return r;
    }

    template <class T>
    static CmdResult<T> Convert(const Token& tok, size_t idx) {
        if constexpr (std::is_same_v<T, std::string>) {
            return TokenToString(tok, idx);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return TokenToInt(tok, idx);
        } else {
            return TokenToFloat(tok, idx);
        }
    }
};

}  // namespace valkey::parser
