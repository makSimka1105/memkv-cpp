#pragma once

#include <utility>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const ListLit& node, EvalContext& ctx) {
    List list;
    list.items.reserve(node.items.size());
    for (const auto& element : node.items) {
        VK_ASSIGN_OR_RETURN(auto value, Eval(element, ctx));
        list.items.push_back(std::move(value));
    }
    return ScriptValue{Box<List>{std::move(list)}};
}

}  // namespace valkey::script
