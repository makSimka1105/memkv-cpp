#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct LRange {
    std::string key;
    int64_t start;
    int64_t stop;
};

using LRangeSchema = schema::Schema<LRange,
    schema::Bind<&LRange::key, schema::Arg<std::string>>,
    schema::Bind<&LRange::start, schema::Arg<int64_t>>,
    schema::Bind<&LRange::stop, schema::Arg<int64_t>>>;

io::Reply ExecLRange(engine::Engine& e, const LRange& x) {
    auto [lst, st] = e.As<types::List>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    const int64_t len = lst->size();
    int64_t from = x.start < 0 ? std::max(int64_t{0}, len + x.start) : x.start;
    int64_t to = x.stop < 0 ? len + x.stop : x.stop;
    to = std::min(to, len - 1);
    if (from > to) {
        return io::MakeArray({});
    }
    std::vector<io::Reply> items;
    items.reserve(to - from + 1);
    for (int64_t i = from; i <= to; ++i) {
        items.push_back(io::MakeBulk((*lst)[i]));
    }
    return io::MakeArray(std::move(items));
}

using LRangeTrait = CommandTrait<"LRANGE", LRangeSchema, &ExecLRange>;

}  // namespace valkey::commands
