#pragma once

#include <string_view>

#include "script/token.hpp"
#include "script/lexer/rule_set.hpp"

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
    static MatchResult Match(Cursor& c) {
        for (auto& [str, op] : kOps)
            if (c.MatchPrefix(str)) return Token{TkOp{op}};
        return std::nullopt;
    }
};

}  // namespace valkey::script
