#pragma once

#include <cstddef>
#include <expected>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "schema/parsers/scan.hpp"

namespace valkey::schema {

template <class Type>
struct Arg {
    using Output = Type;
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;

    static parser::CmdResult<Type> Parse(parser::Cursor& cursor) {
        return ScanValue<Type>(cursor);
    }
};

}  // namespace valkey::schema
