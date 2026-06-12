#pragma once
#include <format>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
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

struct ZRangeByScore {
    std::string key;
    std::string min_str;
    std::string max_str;
    bool withscores;
};

using ZRangeByScoreSchema = schema::Schema<ZRangeByScore,
    schema::Field<&ZRangeByScore::key, schema::Arg<std::string>>,
    schema::Field<&ZRangeByScore::min_str, schema::Arg<std::string>>,
    schema::Field<&ZRangeByScore::max_str, schema::Arg<std::string>>,
    schema::Field<&ZRangeByScore::withscores, schema::Flag<"WITHSCORES">>>;

namespace {

std::optional<double> ParseScoreBound(std::string_view s) {
    if (s == "-inf" || s == "-INF") {
        return -std::numeric_limits<double>::infinity();
    }
    if (s == "+inf" || s == "+INF" || s == "inf" || s == "INF") {
        return std::numeric_limits<double>::infinity();
    }
    try {
        return std::stod(std::string{s});
    } catch (...) {
        return std::nullopt;
    }
}

}  // namespace

io::Reply ExecZRangeByScore(engine::Engine& e, const ZRangeByScore& x) {
    auto min_v = ParseScoreBound(x.min_str);
    auto max_v = ParseScoreBound(x.max_str);
    if (!min_v || !max_v) {
        return io::MakeErr("ERR min or max is not a float");
    }

    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    auto entries = ptr->sl->RangeByScore(*min_v, *max_v);
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

using ZRangeByScoreTrait = CommandTrait<"ZRANGEBYSCORE", ZRangeByScoreSchema, &ExecZRangeByScore>;

}  // namespace valkey::commands
