#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "structures/skiplist.hpp"

namespace valkey::structures {

struct ZSet {
    std::unique_ptr<SkipList<double, std::string>> sl =
        std::make_unique<SkipList<double, std::string>>();
    std::unordered_map<std::string, double> scores;
};

}  // namespace valkey::structures
