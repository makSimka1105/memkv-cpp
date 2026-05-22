#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <type_traits>

#include "parser/cursor.hpp"
#include "parser/error.hpp"

namespace valkey::schema {

template <class T>
std::expected<T, parser::ParseError> ScanValue(parser::Cursor& c) {
    if constexpr (std::is_same_v<T, std::string>) {
        return c.NextString();
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return c.NextInt();
    } else if constexpr (std::is_same_v<T, double>) {
        return c.NextFloat();
    } else {
        static_assert(sizeof(T) == 0, "ScanValue: unsupported type");
    }
}

}  // namespace valkey::schema
