#pragma once

#include <cstddef>
#include <expected>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "schema/combinators/apply_fields.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class Node, FieldFor<Node>... Fields>
struct Seq {
    using Output = Node;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = SaturatingSum(Fields::kMaxArgc...);

    static parser::CmdResult<Node> Parse(parser::Cursor& c) {
        Node node{};
        return ApplyFields<Fields...>(c, node).transform([&] { return std::move(node); });
    }
};

}  // namespace valkey::schema
