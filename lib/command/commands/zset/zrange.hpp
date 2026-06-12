#pragma once
#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/flag.hpp"

namespace valkey::commands {

struct ZRange {
    std::string key;
    int64_t start;
    int64_t stop;
    bool withscores;
};

using ZRangeSchema = schema::Schema<ZRange,
    schema::Field<&ZRange::key, schema::Arg<std::string>>,
    schema::Field<&ZRange::start, schema::Arg<int64_t>>,
    schema::Field<&ZRange::stop, schema::Arg<int64_t>>,
    schema::Field<&ZRange::withscores, schema::Flag<"WITHSCORES">>>;

io::Reply ExecZRange(engine::Engine& e, const ZRange& x) {
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

    auto entries = ptr->sl->RangeByRank(static_cast<size_t>(lo), static_cast<size_t>(hi));
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

using ZRangeTrait = CommandTrait<"ZRANGE", ZRangeSchema, &ExecZRange>;

}  // namespace valkey::commands
