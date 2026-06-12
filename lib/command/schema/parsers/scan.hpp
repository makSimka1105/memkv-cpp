#pragma once
#include <cstdint>
#include <expected>
#include <string>

#include "command/parser/cursor.hpp"
#include "command/diagnostic.hpp"

namespace valkey::schema {
template <class Type>
struct Scan;

template <>
struct Scan<std::string> {
    static parser::CmdResult<std::string> Read(parser::CmdTokenCursor& cursor) {
        return cursor.NextString();
    }
};
template <>
struct Scan<int64_t> {
    static parser::CmdResult<int64_t> Read(parser::CmdTokenCursor& cursor) {
        return cursor.NextInt();
    }
};
template <>
struct Scan<double> {
    static parser::CmdResult<double> Read(parser::CmdTokenCursor& cursor) {
        return cursor.NextFloat();
    }
};
template <class Type>
parser::CmdResult<Type> ScanValue(parser::CmdTokenCursor& cursor) {
    return Scan<Type>::Read(cursor);
}
}  // namespace valkey::schema
