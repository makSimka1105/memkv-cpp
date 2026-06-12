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
            Node* nxt = cur->links[0].to;
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
                path.at[i] = head_;
                path.rank[i] = 0;
                head_->links[i].span = size_;
            }
            current_level_ = lvl;
        }
        Node* node = new Node(std::move(score), std::move(member), lvl);
        for (int i = 0; i < lvl; ++i) {
            size_t left = path.rank[0] - path.rank[i];
            node->links[i].to = path.at[i]->links[i].to;
            node->links[i].span = path.at[i]->links[i].span - left;
            path.at[i]->links[i].to = node;
            path.at[i]->links[i].span = left + 1;
        }
        for (int i = lvl; i < current_level_; ++i) {
            ++path.at[i]->links[i].span;
        }
        ++size_;
    }

    bool Erase(const Score& score, const Member& member) {
        auto path = FindPath(score, member);
        Node* target = path.at[0]->links[0].to;
        if (target == nullptr || target->score != score || target->member != member) {
            return false;
        }
        for (int i = 0; i < current_level_; ++i) {
            if (path.at[i]->links[i].to == target) {
                path.at[i]->links[i].span += target->links[i].span - 1;
                path.at[i]->links[i].to = target->links[i].to;
            } else {
                --path.at[i]->links[i].span;
            }
        }
        delete target;
        --size_;
        while (current_level_ > 1 && head_->links[current_level_ - 1].to == nullptr) {
            --current_level_;
        }
        return true;
    }

    std::vector<std::pair<Score, Member>> PopMin(size_t count = 1) {
        std::vector<std::pair<Score, Member>> result;
        result.reserve(count);
        for (size_t i = 0; i < count && size_ > 0; ++i) {
            Node* target = head_->links[0].to;
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
                while (cur->links[lv].to != nullptr) {
                    cur = cur->links[lv].to;
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
        return CountScoresNotAbove(max_s) - CountScoresBelow(min_s);
    }

    std::optional<size_t> Rank(const Score& score, const Member& member) const {
        const Node* cur = head_;
        size_t traversed = 0;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->links[i].to != nullptr &&
                   IsLess(cur->links[i].to->score, cur->links[i].to->member, score, member)) {
                traversed += cur->links[i].span;
                cur = cur->links[i].to;
            }
        }
        const Node* candidate = cur->links[0].to;
        if (candidate != nullptr && candidate->score == score && candidate->member == member) {
            return traversed;
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
        result.reserve(hi - lo + 1);
        const Node* cur = NodeAtRank(lo);
        for (size_t idx = lo; idx <= hi; ++idx) {
            result.emplace_back(cur->score, cur->member);
            cur = cur->links[0].to;
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
            while (cur->links[i].to != nullptr && cur->links[i].to->score < min_s) {
                cur = cur->links[i].to;
            }
        }
        cur = cur->links[0].to;
        std::vector<std::pair<Score, Member>> result;
        while (cur != nullptr && !(max_s < cur->score)) {
            result.emplace_back(cur->score, cur->member);
            cur = cur->links[0].to;
        }
        return result;
    }

    size_t Size() const { return size_; }

private:
    static constexpr int kMaxLevel = 16;
    static constexpr double kProbability = 0.5;

    struct Node {
        struct Link {
            Node* to = nullptr;
            size_t span = 0;
        };

        Score score;
        Member member;
        std::vector<Link> links;

        Node(Score s, Member m, int level)
            : score(std::move(s)), member(std::move(m)), links(level) {}
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

    struct Path {
        std::array<Node*, kMaxLevel> at;
        std::array<size_t, kMaxLevel> rank;
    };

    Path FindPath(const Score& score, const Member& member) {
        Path path{};
        Node* cur = head_;
        size_t traversed = 0;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->links[i].to != nullptr &&
                   IsLess(cur->links[i].to->score, cur->links[i].to->member, score, member)) {
                traversed += cur->links[i].span;
                cur = cur->links[i].to;
            }
            path.at[i] = cur;
            path.rank[i] = traversed;
        }
        return path;
    }

    size_t CountScoresBelow(const Score& bound) const {
        const Node* cur = head_;
        size_t traversed = 0;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->links[i].to != nullptr && cur->links[i].to->score < bound) {
                traversed += cur->links[i].span;
                cur = cur->links[i].to;
            }
        }
        return traversed;
    }

    size_t CountScoresNotAbove(const Score& bound) const {
        const Node* cur = head_;
        size_t traversed = 0;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->links[i].to != nullptr && !(bound < cur->links[i].to->score)) {
                traversed += cur->links[i].span;
                cur = cur->links[i].to;
            }
        }
        return traversed;
    }

    const Node* NodeAtRank(size_t target) const {
        if (target >= size_) {
            return nullptr;
        }
        const Node* cur = head_;
        size_t traversed = 0;
        for (int i = current_level_ - 1; i >= 0; --i) {
            while (cur->links[i].to != nullptr && traversed + cur->links[i].span <= target) {
                traversed += cur->links[i].span;
                cur = cur->links[i].to;
            }
        }
        return cur->links[0].to;
    }

    Node* head_;
    int current_level_;
    size_t size_;
    std::mt19937 rng_;
};

}  // namespace valkey::structures
