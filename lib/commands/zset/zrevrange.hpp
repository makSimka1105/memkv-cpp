#pragma once
#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/flag.hpp"

namespace valkey::commands {

struct ZRevRange {
    std::string key;
    int64_t start;
    int64_t stop;
    bool withscores;
};

using ZRevRangeSchema = schema::Schema<ZRevRange,
    schema::Bind<&ZRevRange::key, schema::Arg<std::string>>,
    schema::Bind<&ZRevRange::start, schema::Arg<int64_t>>,
    schema::Bind<&ZRevRange::stop, schema::Arg<int64_t>>,
    schema::Bind<&ZRevRange::withscores, schema::Flag<"WITHSCORES">>>;

io::Reply ExecZRevRange(engine::Engine& e, const ZRevRange& x) {
    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    int64_t size = static_cast<int64_t>(ptr->sl->Size());
    int64_t lo = x.start < 0 ? std::max(int64_t{0}, size + x.start) : x.start;
    int64_t hi = x.stop < 0 ? size + x.stop : x.stop;

    if (lo > hi || lo >= size) {
        return io::MakeArray({});
    }

    auto entries = ptr->sl->RangeByRankReverse(static_cast<size_t>(lo), static_cast<size_t>(hi));
    std::vector<io::Reply> items;
    items.reserve(x.withscores ? entries.size() * 2 : entries.size());
    for (const auto& [score, member] : entries) {
        items.push_back(io::MakeBulk(member));
        if (x.withscores) {
            items.push_back(io::MakeBulk(std::format("{:.17g}", score)));
        }
    }
    return io::MakeArray(std::move(items));
}

using ZRevRangeTrait = CommandTrait<"ZREVRANGE", ZRevRangeSchema, &ExecZRevRange>;

}  // namespace valkey::commands
