#pragma once

#include <cstddef>
#include <expected>
#include <vector>

#include "meta/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/apply.hpp"
#include "schema/contract.hpp"

namespace valkey::schema {

template <class ElemNode, RuleFor<ElemNode>... Rules>
struct RepeatValues {
    using Output = std::vector<ElemNode>;
    static constexpr size_t kMinArgc = SaturatingSum(Rules::kMinArgc...);
    static constexpr size_t kMaxArgc = kUnbounded;

    static std::expected<std::vector<ElemNode>, parser::ParseError> Parse(parser::Cursor& c) {
        std::vector<ElemNode> out;
        while (!c.Done()) {
            ElemNode e{};
            size_t before = c.pos;
            auto r = ApplyRules<Rules...>(c, e);
            if (!r) {
                return std::unexpected(r.error());
            }
            if (c.pos == before) {
                break;
            }
            out.push_back(std::move(e));
        }
        return out;
    }
};

template <class ElemNode, class... Rules>
using RepeatStrategy = RepeatValues<ElemNode, Rules...>;

}  // namespace valkey::schema
