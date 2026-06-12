#pragma once

#include <string_view>
#include <utility>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

constexpr std::pair<std::string_view, TkOp::OpKind> kOps[] = {
    {"==", TkOp::kEqEq},
    {"!=", TkOp::kNotEq},
    {"<=", TkOp::kLessEq},
    {">=", TkOp::kGreaterEq},
    {"+",  TkOp::kPlus},
    {"-",  TkOp::kMinus},
    {"*",  TkOp::kStar},
    {"/",  TkOp::kSlash},
    {"%",  TkOp::kPercent},
    {"=",  TkOp::kAssign},
    {"<",  TkOp::kLess},
    {">",  TkOp::kGreater},
};

struct OpRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        for (auto& [str, op] : kOps) {
            if (c.MatchPrefix(str)) {
                return Token{TkOp{op}};
            }
        }
        return std::nullopt;
    }
};

}  // namespace valkey::script
