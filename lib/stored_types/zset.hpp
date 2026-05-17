#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "structures/skiplist.hpp"

namespace valkey::storage {

struct ZSet {
    std::unique_ptr<structures::SkipList<double, std::string>> sl =
        std::make_unique<structures::SkipList<double, std::string>>();
    std::unordered_map<std::string, double> scores;
};

}  // namespace valkey::storage

namespace valkey::stored_types::zset {

struct Trait {
    using Type = storage::ZSet;
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
