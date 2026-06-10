#pragma once

#include <variant>

#include "script/ast.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const IntLit&, EvalContext&);
ValueResult Eval(const DblLit&, EvalContext&);
ValueResult Eval(const StrLit&, EvalContext&);
ValueResult Eval(const BoolLit&, EvalContext&);
ValueResult Eval(const NilLit&, EvalContext&);
ValueResult Eval(const Ident&, EvalContext&);
ValueResult Eval(const BinOp&, EvalContext&);
ValueResult Eval(const UnOp&, EvalContext&);
ValueResult Eval(const Call&, EvalContext&);
ValueResult Eval(const ScriptCall&, EvalContext&);
ValueResult Eval(const Index&, EvalContext&);
ValueResult Eval(const ListLit&, EvalContext&);

ExecResult Exec(const LetStmt&, EvalContext&);
ExecResult Exec(const AssignStmt&, EvalContext&);
ExecResult Exec(const IfStmt&, EvalContext&);
ExecResult Exec(const WhileStmt&, EvalContext&);
ExecResult Exec(const ForStmt&, EvalContext&);
ExecResult Exec(const ReturnStmt&, EvalContext&);
ExecResult Exec(const ExprStmt&, EvalContext&);
ExecResult Exec(const Block&, EvalContext&);

ValueResult Eval(const Expr& expr, EvalContext& ctx) {
    return std::visit([&](const auto& node) { return Eval(node, ctx); }, expr);
}

ExecResult Exec(const Stmt& stmt, EvalContext& ctx) {
    return std::visit([&](const auto& node) { return Exec(node, ctx); }, stmt);
}

}  // namespace valkey::script
