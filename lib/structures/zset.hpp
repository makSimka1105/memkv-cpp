#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "structures/skiplist.hpp"

namespace valkey::structures {

struct ZSet {
    std::unique_ptr<SkipList<double, std::string_view>> sl =
        std::make_unique<SkipList<double, std::string_view>>();
    std::unordered_map<std::string, double> scores;

    bool SetScore(const std::string& member, double score) {
        auto [it, inserted] = scores.try_emplace(member, score);
        if (!inserted) {
            sl->Erase(it->second, it->first);
            it->second = score;
        }
        sl->Insert(score, it->first);
        return inserted;
    }

    bool RemoveMember(const std::string& member) {
        auto it = scores.find(member);
        if (it == scores.end()) {
            return false;
        }
        sl->Erase(it->second, it->first);
        scores.erase(it);
        return true;
    }

    std::vector<std::pair<double, std::string>> PopMax(size_t count) {
        return Materialize(sl->PopMax(count));
    }

    std::vector<std::pair<double, std::string>> PopMin(size_t count) {
        return Materialize(sl->PopMin(count));
    }

private:
    std::vector<std::pair<double, std::string>> Materialize(
        const std::vector<std::pair<double, std::string_view>>& popped) {
        std::vector<std::pair<double, std::string>> result;
        result.reserve(popped.size());
        for (const auto& [score, member] : popped) {
            result.emplace_back(score, std::string(member));
        }
        for (const auto& [score, member] : result) {
            scores.erase(member);
        }
        return result;
    }
};

}  // namespace valkey::structures
