#pragma once

#include <cstddef>
#include <cstdint>
#include <variant>

#include "meta/try.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/error.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/value.hpp"

namespace valkey::script {

ValueResult Eval(const Index& node, EvalContext& ctx) {
    VK_ASSIGN_OR_RETURN(auto object, Eval(*node.object, ctx));
    VK_ASSIGN_OR_RETURN(auto key, Eval(*node.key, ctx));
    const auto* list = std::get_if<Box<List>>(&object);
    if (!list) {
        return MakeError(ErrKind::kType, "indexing requires a list");
    }
    const auto* position = std::get_if<int64_t>(&key);
    if (!position) {
        return MakeError(ErrKind::kType, "list index must be an integer");
    }
    const auto& items = (*list)->items;
    if (*position < 0) {
        return MakeError(ErrKind::kIndex, "list index out of range");
    }
    size_t offset = *position;
    if (offset >= items.size()) {
        return MakeError(ErrKind::kIndex, "list index out of range");
    }
    return items[offset];
}

}  // namespace valkey::script
