#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "script/box.hpp"

namespace valkey::script {

struct Expr;
struct Stmt;

struct IntLit {
    int64_t value;
};
struct DblLit {
    double value;
};
struct StrLit {
    std::string value;
};
struct BoolLit {
    bool value;
};
struct NilLit {};

struct Ident {
    std::string name;
};

struct BinOp {
    enum Op {
        kAdd,
        kSub,
        kMul,
        kDiv,
        kMod,
        kEq,
        kNeq,
        kLt,
        kLe,
        kGt,
        kGe,
        kAnd,
        kOr,
        kLast,
    };
    Op op;
    Box<Expr> lhs;
    Box<Expr> rhs;
};

struct UnOp {
    enum Kind { kNeg, kNot, kLast };
    Kind op;
    Box<Expr> operand;
};

struct Call {
    std::string callee;
    std::vector<Expr> args;
};

struct ScriptCall {
    std::string callee;
    std::vector<Expr> args;
};

struct Index {
    Box<Expr> object;
    Box<Expr> key;
};

struct ListLit {
    std::vector<Expr> items;
};

struct Expr : std::variant<IntLit, DblLit, StrLit, BoolLit, NilLit, Ident, BinOp, UnOp,
                           Call, ScriptCall, Index, ListLit> {
    using variant::variant;
};

struct LetStmt {
    std::string name;
    Expr init;
};
struct AssignStmt {
    std::string name;
    Expr value;
};

struct IfStmt {
    Expr cond;
    Box<Stmt> then_;
    std::optional<Box<Stmt>> else_;
};

struct WhileStmt {
    Expr cond;
    Box<Stmt> body;
};

struct ForStmt {
    std::string var;
    Expr iter;
    Box<Stmt> body;
};

struct ReturnStmt {
    std::optional<Expr> value;
};
struct ExprStmt {
    Expr expr;
};

struct Block {
    std::vector<Stmt> stmts;
};

struct Stmt
    : std::variant<LetStmt, AssignStmt, IfStmt, WhileStmt, ForStmt, ReturnStmt, ExprStmt, Block> {
    using variant::variant;
};

struct Program {
    std::vector<Stmt> stmts;
};

}  // namespace valkey::script
