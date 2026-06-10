#pragma once

#include <concepts>
#include <span>
#include <string_view>

#include "meta/match_first.hpp"
#include "script/evaluator/builtins/call.hpp"
#include "script/evaluator/builtins/len.hpp"
#include "script/evaluator/builtins/type.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/value.hpp"

namespace valkey::script {

using Builtin = ValueResult (*)(std::span<const ScriptValue>, EvalContext&);

template <typename T>
concept BuiltinLike = requires(std::span<const ScriptValue> args, EvalContext& ctx) {
    { T::kName } -> std::convertible_to<std::string_view>;
    { T::Apply(args, ctx) } -> std::same_as<ValueResult>;
};

template <BuiltinLike... Ts>
struct BuiltinRegistry {
    static Builtin Find(std::string_view name) {
        Builtin fn = nullptr;
        MatchFirst<Ts...>([&]<size_t, typename T>() {
            if (T::kName != name) {
                return false;
            }
            fn = &T::Apply;
            return true;
        });
        return fn;
    }
};

using ActiveBuiltins = BuiltinRegistry<CallBuiltin, LenBuiltin, TypeBuiltin>;

inline Builtin FindBuiltin(std::string_view name) {
    return ActiveBuiltins::Find(name);
}

}  // namespace valkey::script
