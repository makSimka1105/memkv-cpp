#pragma once
#include <cstddef>
#include <expected>
#include <utility>
#include <vector>
#include "meta/try.hpp"
#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "schema/core/argc.hpp"
#include "schema/parsers/scan.hpp"
namespace valkey::schema {
template <class Type, size_t Min = 0>
struct Variadic {
    using Output = std::vector<Type>;
    static constexpr size_t kMinArgc = Min;
    static constexpr size_t kMaxArgc = kUnbounded;
    static parser::CmdResult<std::vector<Type>> Parse(parser::Cursor& cursor) {
        std::vector<Type> out_elements;
        while (!cursor.Done()) {
            VK_ASSIGN_OR_RETURN(auto&& value, ScanValue<Type>(cursor));
            out_elements.push_back(std::move(value));
        }
        return out_elements;
    }
};
}  // namespace valkey::schema
