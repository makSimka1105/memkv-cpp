#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <utility>
#include <vector>

namespace valkey::structures {

template <typename Score, typename Member>
class SkipList {
public:
    explicit SkipList(uint32_t seed = 42) : current_level_(1), size_(0), rng_(seed) {
        head_ = new Node(Score{}, Member{}, kMaxLevel);
    }

    ~SkipList() {
        Node* cur = head_;
        while (cur != nullptr) {
            Node* nxt = cur->next[0];
            delete cur;
            cur = nxt;
        }
    }

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    void Insert(Score score, Member member) {
        auto path = FindPath(score, member);
        int lvl = RandomLevel();
        if (lvl > current_level_) {
            for (int i = current_level_; i < lvl; ++i) {
                path[i] = head_;
            }
            current_level_ = lvl;
        }
        Node* node = new Node(std::move(score), std::move(member), lvl);
        for (int i = 0; i < lvl; ++i) {
            node->next[i] = path[i]->next[i];
            path[i]->next[i] = node;
        }
        ++size_;
    }

    bool Erase(const Score& score, const Member& member) {
        auto path = FindPath(score, member);
        Node* target = path[0]->next[0];
        if (target == nullptr || target->score != score || target->member != member) {
            return false;
        }
        for (int i = 0; i < current_level_; ++i) {
            if (path[i]->next[i] != target) {
                break;
            }
            path[i]->next[i] = target->next[i];
        }
        delete target;
        --size_;
        while (current_level_ > 1 && head_->next[current_level_ - 1] == nullptr) {
            --current_level_;
        }
        return true;
    }

    std::vector<std::pair<Score, Member>> PopMin(size_t count = 1) {
        std::vector<std::pair<Score, Member>> result;
        result.reserve(count);
        for (size_t i = 0; i < count && size_ > 0; ++i) {
            Node* target = head_->next[0];
            if (target == nullptr) {
                break;
            }
            result.emplace_back(target->score, target->member);
            Erase(target->score, target->member);
        }
        return result;
    }

    std::vector<std::pair<Score, Member>> PopMax(size_t count = 1) {
        std::vector<std::pair<Score, Member>> result;
        result.reserve(count);
        for (size_t i = 0; i < count && size_ > 0; ++i) {
            Node* cur = head_;
            for (int lv = current_level_ - 1; lv >= 0; --lv) {
                while (cur->next[lv] != nullptr) {
                    cur = cur->next[lv];
                }
            }
            if (cur == head_) {
                break;
            }
            result.emplace_back(cur->score, cur->member);
            Erase(cur->score, cur->member);
        }
        return result;
    }

    size_t CountByScore(const Score& min_s, const Score& max_s) const {
        if (max_s < min_s) {
            return 0;
        }
        Node* cur = head_;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->next[i] != nullptr && cur->next[i]->score < min_s) {
                cur = cur->next[i];
            }
        }
        cur = cur->next[0];
        size_t count = 0;
        while (cur != nullptr && !(max_s < cur->score)) {
            ++count;
            cur = cur->next[0];
        }
        return count;
    }

    std::optional<size_t> Rank(const Score& score, const Member& member) const {
        size_t idx = 0;
        for (const Node* cur = head_->next[0]; cur != nullptr; cur = cur->next[0]) {
            if (cur->score == score && cur->member == member) {
                return idx;
            }
            ++idx;
        }
        return std::nullopt;
    }

    std::optional<size_t> RankReverse(const Score& score, const Member& member) const {
        auto r = Rank(score, member);
        if (!r) {
            return std::nullopt;
        }
        return size_ - 1 - *r;
    }

    std::vector<std::pair<Score, Member>> RangeByRank(size_t lo, size_t hi) const {
        if (lo > hi || lo >= size_) {
            return {};
        }
        hi = std::min(hi, size_ - 1);
        std::vector<std::pair<Score, Member>> result;
        const Node* cur = head_->next[0];
        size_t idx = 0;
        while (cur != nullptr && idx <= hi) {
            if (idx >= lo) {
                result.emplace_back(cur->score, cur->member);
            }
            cur = cur->next[0];
            ++idx;
        }
        return result;
    }

    std::vector<std::pair<Score, Member>> RangeByRankReverse(size_t lo, size_t hi) const {
        auto v = RangeByRank(lo, hi);
        std::ranges::reverse(v);
        return v;
    }

    std::vector<std::pair<Score, Member>> RangeByScore(const Score& min_s,
                                                       const Score& max_s) const {
        if (max_s < min_s) {
            return {};
        }
        Node* cur = head_;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->next[i] != nullptr && cur->next[i]->score < min_s) {
                cur = cur->next[i];
            }
        }
        cur = cur->next[0];
        std::vector<std::pair<Score, Member>> result;
        while (cur != nullptr && !(max_s < cur->score)) {
            result.emplace_back(cur->score, cur->member);
            cur = cur->next[0];
        }
        return result;
    }

    std::vector<std::pair<Score, Member>> All() const {
        if (size_ == 0) {
            return {};
        }
        return RangeByRank(0, size_ - 1);
    }

    size_t Size() const { return size_; }

private:
    static constexpr int kMaxLevel = 16;
    static constexpr double kProbability = 0.5;

    struct Node {
        Score score;
        Member member;
        std::vector<Node*> next;

        Node(Score s, Member m, int level)
            : score(std::move(s)), member(std::move(m)), next(level, nullptr) {}
    };

    bool IsLess(const Score& s1, const Member& m1, const Score& s2, const Member& m2) const {
        if (s1 != s2) {
            return s1 < s2;
        }
        return m1 < m2;
    }

    int RandomLevel() {
        int level = 1;
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        while (level < kMaxLevel && dist(rng_) < kProbability) {
            ++level;
        }
        return level;
    }

    std::array<Node*, kMaxLevel> FindPath(const Score& score, const Member& member) {
        std::array<Node*, kMaxLevel> path{};
        Node* cur = head_;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->next[i] != nullptr &&
                   IsLess(cur->next[i]->score, cur->next[i]->member, score, member)) {
                cur = cur->next[i];
            }
            path[i] = cur;
        }
        return path;
    }

    Node* head_;
    int current_level_;
    size_t size_;
    std::mt19937 rng_;
};

}  // namespace valkey::structures
