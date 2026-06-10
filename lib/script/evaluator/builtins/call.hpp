#pragma once

#include <span>
#include <string_view>

#include "engine/engine.hpp"
#include "meta/try.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/evaluator/core/convert.hpp"
#include "script/value.hpp"

namespace valkey::script {

struct CallBuiltin {
    static constexpr std::string_view kName = "call";
    static ValueResult Apply(std::span<const ScriptValue> args, EvalContext& ctx) {
        if (args.empty()) {
            return MakeError(ErrKind::kArity, "call expects a command name");
        }
        VK_ASSIGN_OR_RETURN(auto argv, ArgsToStrings(args));
        return FromReply(ctx.engine.EvalRaw(argv));
    }
};

}  // namespace valkey::script
