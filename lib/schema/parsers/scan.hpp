#pragma once
#include <cstdint>
#include <expected>
#include <string>

#include "parser/cursor.hpp"
#include "diagnostic.hpp"
namespace valkey::schema {
template <class Type>
struct Scan;
template <>
struct Scan<std::string> {
    static parser::CmdResult<std::string> Read(parser::Cursor& cursor) {
        return cursor.NextString();
    }
};
template <>
struct Scan<int64_t> {
    static parser::CmdResult<int64_t> Read(parser::Cursor& cursor) {
        return cursor.NextInt();
    }
};
template <>
struct Scan<double> {
    static parser::CmdResult<double> Read(parser::Cursor& cursor) {
        return cursor.NextFloat();
    }
};
template <class Type>
parser::CmdResult<Type> ScanValue(parser::Cursor& cursor) {
    return Scan<Type>::Read(cursor);
}
}  // namespace valkey::schema
