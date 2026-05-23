#pragma once

#include <cstddef>
#include <deque>
#include <string>
#include <string_view>

namespace valkey::types {

struct ListTrait {
    using Type = std::deque<std::string>;
    static constexpr std::string_view kName = "list";
    static size_t SizeOf(const Type& lst) {
        size_t total = sizeof(Type);
        for (const auto& s : lst) {
            total += sizeof(std::string) + s.size();
        }
        return total;
    }
    static bool IsEmpty(const Type& lst) { return lst.empty(); }
};

}  // namespace valkey::types
