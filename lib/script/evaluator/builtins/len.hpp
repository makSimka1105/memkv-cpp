#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "script/box.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/value.hpp"

namespace valkey::script {

struct LenBuiltin {
    static constexpr std::string_view kName = "len";
    static ValueResult Apply(std::span<const ScriptValue> args, EvalContext&) {
        if (args.size() != 1) {
            return MakeError(ErrKind::kArity, "len expects 1 argument");
        }
        if (const auto* text = std::get_if<std::string>(&args[0])) {
            return static_cast<int64_t>(text->size());
        }
        if (const auto* list = std::get_if<Box<List>>(&args[0])) {
            return static_cast<int64_t>((*list)->items.size());
        }
        return MakeError(ErrKind::kType, "len expects a string or list");
    }
};

}  // namespace valkey::script
