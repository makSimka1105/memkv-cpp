#pragma once

#include "script/ast.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const IntLit& node, EvalContext&) { return node.value; }
ValueResult Eval(const DblLit& node, EvalContext&) { return node.value; }
ValueResult Eval(const StrLit& node, EvalContext&) { return node.value; }
ValueResult Eval(const BoolLit& node, EvalContext&) { return node.value; }
ValueResult Eval(const NilLit&, EvalContext&) { return Nil{}; }

}  // namespace valkey::script
