#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace valkey::script {

struct Span {
    uint32_t lo;
    uint32_t hi;
};

struct TkInt {
    int64_t value;
};

struct TkDouble {
    double value;
};

struct TkStr {
    std::string value;
};

struct TkIdent {
    std::string name;
};

struct TkKw {
    enum Keyword {
        kLet,
        kIf,
        kElse,
        kWhile,
        kFor,
        kIn,
        kReturn,
        kAnd,
        kOr,
        kNot,
        kNil,
        kTrue,
        kFalse,
    };
    Keyword kw;
};

struct TkOp {
    enum OpKind {
        kPlus,
        kMinus,
        kStar,
        kSlash,
        kPercent,
        kAssign,
        kEqEq,
        kNotEq,
        kLess,
        kLessEq,
        kGreater,
        kGreaterEq,
    };
    OpKind op;
};

struct TkLParen {};
struct TkRParen {};
struct TkLBrace {};
struct TkRBrace {};
struct TkLBrack {};
struct TkRBrack {};
struct TkComma {};
struct TkSemi {};
struct TkAt {};
struct TkEof {};

using Token = std::variant<TkInt, TkDouble, TkStr, TkIdent, TkKw, TkOp, TkLParen, TkRParen,
                           TkLBrace, TkRBrace, TkLBrack, TkRBrack, TkComma, TkSemi, TkAt, TkEof>;

struct SpannedToken {
    Token tok;
    Span span;
};

}  // namespace valkey::script
