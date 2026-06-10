#pragma once

#include <string>
#include <utility>
#include <vector>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/error.hpp"
#include "script/evaluator/builtins/registry.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const Call& node, EvalContext& ctx) {
    std::vector<ScriptValue> args;
    args.reserve(node.args.size());
    for (const auto& argument : node.args) {
        VK_ASSIGN_OR_RETURN(auto value, Eval(argument, ctx));
        args.push_back(std::move(value));
    }

    if (Builtin builtin = FindBuiltin(node.callee)) {
        return builtin(args, ctx);
    }

    return MakeError(ErrKind::kName, "unknown builtin '" + node.callee + "'");
}

}  // namespace valkey::script
