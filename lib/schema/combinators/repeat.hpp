#pragma once

#include <cstddef>
#include <expected>
#include <utility>
#include <vector>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "meta/try.hpp"
#include "schema/combinators/apply_fields.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class Elem, FieldFor<Elem>... Fields>
struct Repeat {
    using Output = std::vector<Elem>;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = kUnbounded;

    static parser::CmdResult<std::vector<Elem>> Parse(parser::Cursor& cursor) {
        std::vector<Elem> out;
        while (!cursor.Done()) {
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
