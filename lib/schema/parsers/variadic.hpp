#pragma once
#include <cstddef>
#include <expected>
#include <utility>
#include <vector>
#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "schema/core/argc.hpp"
#include "schema/parsers/scan.hpp"
namespace valkey::schema {
template <class Type, size_t Min = 0>
struct Variadic {
    using Output = std::vector<Type>;
    static constexpr size_t kMinArgc = Min;
    static constexpr size_t kMaxArgc = kUnbounded;
    static std::expected<std::vector<Type>, parser::ParseError> Parse(parser::Cursor& cursor) {
        std::vector<Type> out_elements;
        while (!cursor.Done()) {
            auto result = ScanValue<Type>(cursor);
            if (!result) {
                return std::unexpected(result.error());
            }
            out_elements.push_back(std::move(*result));
        }
        return out_elements;
    }
};
}  // namespace valkey::schema
