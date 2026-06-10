#pragma once

#include <vector>

#include "engine/engine.hpp"
#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/convert.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const ScriptCall& node, EvalContext& ctx) {
    std::vector<ScriptValue> args;
    args.reserve(node.args.size());
    for (const auto& argument : node.args) {
        VK_ASSIGN_OR_RETURN(auto value, Eval(argument, ctx));
        args.push_back(std::move(value));
    }

    const engine::ScriptFn* fn = ctx.engine.FindFunction(node.callee);
    if (!fn) {
        return MakeError(ErrKind::kName, "no such function '@" + node.callee + "'");
    }
    return CallScriptFn(*fn, args);
}

}  // namespace valkey::script
