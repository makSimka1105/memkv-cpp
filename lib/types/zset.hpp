#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "structures/zset.hpp"

namespace valkey::types {

struct ZSetTrait {
    using Type = structures::ZSet;
    static constexpr std::string_view kName = "zset";
    static size_t ElementBytes(std::string_view m) {
        return sizeof(std::string) + m.size() + sizeof(double) + 32;
    }
    static size_t SizeOf(const Type& z) {
        size_t total = sizeof(Type);
        for (const auto& [m, _] : z.scores) {
            total += ElementBytes(m);
        }
        return total;
    }
    static bool IsEmpty(const Type& z) { return z.scores.empty(); }
};

}  // namespace valkey::types
