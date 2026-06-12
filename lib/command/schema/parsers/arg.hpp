#pragma once

#include <cstddef>
#include <expected>

#include "command/parser/cursor.hpp"
#include "command/schema/parsers/scan.hpp"
#include "command/diagnostic.hpp"

namespace valkey::schema {

template <class Type>
struct Arg {
    using Output = Type;
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;

    static parser::CmdResult<Type> Parse(parser::CmdTokenCursor& cursor) {
        return ScanValue<Type>(cursor);
    }
};

}  // namespace valkey::schema
