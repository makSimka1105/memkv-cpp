#pragma once

#include <cstddef>
#include <expected>
#include <utility>
#include <vector>

#include "command/parser/cursor.hpp"
#include "command/schema/combinators/apply_fields.hpp"
#include "command/schema/core/argc.hpp"
#include "command/schema/core/concepts.hpp"
#include "command/diagnostic.hpp"
#include "meta/try.hpp"

namespace valkey::schema {

template <class Elem, FieldFor<Elem>... Fields>
struct Repeat {
    using Output = std::vector<Elem>;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = kUnbounded;

    static parser::CmdResult<std::vector<Elem>> Parse(parser::CmdTokenCursor& cursor) {
        std::vector<Elem> out;
        while (!cursor.AtEnd()) {
            Elem e{};
            size_t before = cursor.Pos();
            VK_RETURN_IF_ERROR(ApplyFields<Fields...>(cursor, e));
            if (cursor.Pos() == before) {
                break;
            }
            out.push_back(std::move(e));
        }
        return out;
    }
};

}  // namespace valkey::schema
