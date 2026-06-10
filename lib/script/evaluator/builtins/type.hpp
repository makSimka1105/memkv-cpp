#pragma once

#include <span>
#include <string>
#include <string_view>

#include "script/error.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/value.hpp"

namespace valkey::script {

struct TypeBuiltin {
    static constexpr std::string_view kName = "type";
    static ValueResult Apply(std::span<const ScriptValue> args, EvalContext&) {
        if (args.size() != 1) {
            return MakeError(ErrKind::kArity, "type expects 1 argument");
        }
        return std::string(TypeName(args[0]));
    }
};

}  // namespace valkey::script
