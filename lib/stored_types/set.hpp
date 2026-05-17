#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>

namespace valkey::stored_types::set {

struct Trait {
    using Type = std::unordered_set<std::string>;
    static constexpr std::string_view kName = "set";
    static size_t SizeOf(const Type& s) {
        size_t total = sizeof(Type);
        for (const auto& e : s) {
            total += sizeof(std::string) + e.size();
        }
        return total;
    }
    static bool IsEmpty(const Type& s) { return s.empty(); }
};

}  // namespace valkey::stored_types::set
