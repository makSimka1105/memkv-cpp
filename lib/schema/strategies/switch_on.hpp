#pragma once

#include <cstddef>
#include <expected>

#include "schema/alt_group.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/command_trait.hpp"

namespace valkey::schema {

template <class... Cases>
struct SwitchOn {
    using Group = AltGroup<Cases...>;
    using Output = typename Group::Variant;
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = kUnbounded;

    static std::expected<Output, parser::ParseError> Parse(parser::Cursor& c) {
        return Group::Parse(c);
    }
};

}  // namespace valkey::schema
