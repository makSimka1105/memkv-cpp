#pragma once
#include <cstdint>
#include <expected>
#include <string>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
namespace valkey::schema {
template <class Type>
struct Scan;
template <>
struct Scan<std::string> {
    static std::expected<std::string, parser::ParseError> Read(parser::Cursor& cursor) {
        return cursor.NextString();
    }
};
template <>
struct Scan<int64_t> {
    static std::expected<int64_t, parser::ParseError> Read(parser::Cursor& cursor) {
        return cursor.NextInt();
    }
};
template <>
struct Scan<double> {
    static std::expected<double, parser::ParseError> Read(parser::Cursor& cursor) {
        return cursor.NextFloat();
    }
};
template <class Type>
std::expected<Type, parser::ParseError> ScanValue(parser::Cursor& cursor) {
    return Scan<Type>::Read(cursor);
}
}  // namespace valkey::schema
