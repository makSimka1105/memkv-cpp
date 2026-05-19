#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "structures/zset.hpp"

namespace valkey::stored_types::zset {

struct Trait {
    using Type = structures::ZSet;
    static constexpr std::string_view kName = "zset";
    static size_t SizeOf(const Type& z) {
        size_t total = sizeof(Type);
        for (const auto& [m, _] : z.scores) {
            total += sizeof(std::string) + m.size() + sizeof(double) + 32;
        }
        return total;
    }
    static bool IsEmpty(const Type& z) { return z.scores.empty(); }
};

}  // namespace valkey::stored_types::zset
