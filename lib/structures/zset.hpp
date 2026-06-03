#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "structures/skiplist.hpp"

namespace valkey::structures {

struct ZSet {
    std::unique_ptr<SkipList<double, std::string>> sl =
        std::make_unique<SkipList<double, std::string>>();
    std::unordered_map<std::string, double> scores;

    bool SetScore(const std::string& member, double score) {
        auto [it, inserted] = scores.try_emplace(member, score);
        if (!inserted) {
            sl->Erase(it->second, member);
            it->second = score;
        }
        sl->Insert(score, member);
        return inserted;
    }

    bool RemoveMember(const std::string& member) {
        auto it = scores.find(member);
        if (it == scores.end()) {
            return false;
        }
        sl->Erase(it->second, member);
        scores.erase(it);
        return true;
    }

    std::vector<std::pair<double, std::string>> PopMax(size_t count) {
        auto popped = sl->PopMax(count);
        for (const auto& [score, member] : popped) {
            scores.erase(member);
        }
        return popped;
    }

    std::vector<std::pair<double, std::string>> PopMin(size_t count) {
        auto popped = sl->PopMin(count);
        for (const auto& [score, member] : popped) {
            scores.erase(member);
        }
        return popped;
    }
};

}  // namespace valkey::structures
