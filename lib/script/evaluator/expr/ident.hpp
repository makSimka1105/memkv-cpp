#pragma once

#include <string>

#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const Ident& node, EvalContext& ctx) {
    if (const ScriptValue* value = ctx.env.Lookup(node.name)) {
        return *value;
    }
    return MakeError(ErrKind::kName, "undefined variable '" + node.name + "'");
}

}  // namespace valkey::script
