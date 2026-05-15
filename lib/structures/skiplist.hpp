#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <utility>
#include <vector>

namespace valkey::structures {

template <typename Score, typename Member>
class SkipList {
private:
    static constexpr int kMaxLevel = 16;
    static constexpr double kProbability = 0.5;

    struct Node {
        Score score;
        Member member;
        std::vector<Node*> next;

        Node(Score s, Member m, int level) : score(s), member(m), next(level, nullptr) {}
    };

    Node* head_;
    int current_max_level_;
    size_t size_;
    std::mt19937 rng_;

    bool IsLess(const Score& s1, const Member& m1, const Score& s2, const Member& m2) const {
        if (s1 != s2) {
            return s1 < s2;
        }
        return m1 < m2;
    }

    int DesideLevel() {
        int level = 1;
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        while (level < kMaxLevel && dist(rng_) < kProbability) {
            level++;
        }
        return level;
    }

    std::vector<Node*> GetPath(const Score& score, const Member& member) const {
        std::vector<Node*> path(kMaxLevel, nullptr);
        Node* current = head_;

        for (int i = current_max_level_ - 1; i >= 0; i--) {
            while (current->next[i] != nullptr &&
                   IsLess(current->next[i]->score, current->next[i]->member, score, member)) {
                current = current->next[i];
            }
            path[i] = current;
        }
        return path;
    }

public:
    explicit SkipList(uint32_t seed = 42) : current_max_level_(1), size_(0), rng_(seed) {
        head_ = new Node(Score{}, Member{}, kMaxLevel);
    }

    ~SkipList() {
        Node* current = head_;
        while (current != nullptr) {
            Node* next_node = current->next[0];
            delete current;
            current = next_node;
        }
    }

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
    SkipList(SkipList&&) = delete;
    SkipList& operator=(SkipList&&) = delete;

    void Insert(Score score, Member member) {
        std::vector<Node*> path = GetPath(score, member);
        int new_level = DesideLevel();

        if (new_level > current_max_level_) {
            for (int i = current_max_level_; i < new_level; i++) {
                path[i] = head_;
            }
            current_max_level_ = new_level;
        }

        Node* new_node = new Node(score, member, new_level);
        for (int i = 0; i < new_level; i++) {
            new_node->next[i] = path[i]->next[i];
            path[i]->next[i] = new_node;
        }
        size_++;
    }

    bool Erase(const Score& score, const Member& member) {
        std::vector<Node*> path = GetPath(score, member);
        Node* target = path[0]->next[0];

        if (target == nullptr || target->score != score || target->member != member) {
            return false;
        }

        for (int i = 0; i < current_max_level_; i++) {
            if (path[i]->next[i] != target) {
                break;
            }
            path[i]->next[i] = target->next[i];
        }

        delete target;
        size_--;

        while (current_max_level_ > 1 && head_->next[current_max_level_ - 1] == nullptr) {
            current_max_level_--;
        }
        return true;
    }

    std::vector<std::pair<Score, Member>> RangeByScore(const Score& min_s,
                                                       const Score& max_s) const {
        if (max_s < min_s) {
            return {};
        }

        Node* current = head_;
        for (int i = current_max_level_ - 1; i >= 0; i--) {
            while (current->next[i] != nullptr && current->next[i]->score < min_s) {
                current = current->next[i];
            }
        }
        current = current->next[0];

        std::vector<std::pair<Score, Member>> result;
        while (current != nullptr && !(max_s < current->score)) {
            result.emplace_back(current->score, current->member);
            current = current->next[0];
        }
        return result;
    }

    std::optional<size_t> Rank(const Score& score, const Member& member) const {
        size_t index = 0;
        for (const Node* current = head_->next[0]; current != nullptr; current = current->next[0]) {
            if (current->score == score && current->member == member) {
                return index;
            }
            index++;
        }
        return std::nullopt;
    }

    std::vector<std::pair<Score, Member>> RangeByRank(size_t lo, size_t hi) const {
        if (lo > hi || lo >= size_) {
            return {};
        }
        hi = std::min(hi, size_ - 1);

        std::vector<std::pair<Score, Member>> result;
        const Node* current = head_->next[0];
        size_t current_index = 0;

        while (current != nullptr && current_index <= hi) {
            if (current_index >= lo) {
                result.emplace_back(current->score, current->member);
            }
            current = current->next[0];
            current_index++;
        }
        return result;
    }

    size_t Size() const { return size_; }
};

}  // namespace valkey::structures
