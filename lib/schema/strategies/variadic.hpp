#pragma once

#include <cstddef>
#include <expected>
#include <utility>
#include <vector>

#include "meta/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/scan.hpp"

namespace valkey::schema {

template <class T, size_t Min = 0>
struct Variadic {
    using Output = std::vector<T>;
    static constexpr size_t kMinArgc = Min;
    static constexpr size_t kMaxArgc = kUnbounded;

    static std::expected<std::vector<T>, parser::ParseError> Parse(parser::Cursor& c) {
        std::vector<T> out;
        while (!c.Done()) {
            auto r = ScanValue<T>(c);
            if (!r) {
                return std::unexpected(r.error());
            }
            out.push_back(std::move(*r));
        }
        return out;
    }
};

}  // namespace valkey::schema
