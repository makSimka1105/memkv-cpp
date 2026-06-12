#pragma once

#include <cstddef>
#include <expected>

#include "command/parser/cursor.hpp"
#include "command/schema/combinators/apply_fields.hpp"
#include "command/schema/core/argc.hpp"
#include "command/schema/core/concepts.hpp"
#include "command/diagnostic.hpp"

namespace valkey::schema {

template <class Node, FieldFor<Node>... Fields>
struct Seq {
    using Output = Node;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = SaturatingSum(Fields::kMaxArgc...);

    static parser::CmdResult<Node> Parse(parser::CmdTokenCursor& c) {
        Node node{};
        return ApplyFields<Fields...>(c, node).transform([&] { return std::move(node); });
    }
};

}  // namespace valkey::schema
