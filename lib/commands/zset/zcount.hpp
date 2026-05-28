#pragma once
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct ZCount {
    std::string key;
    std::string min_str;
    std::string max_str;
};

using ZCountSchema = schema::Schema<ZCount,
    schema::Bind<&ZCount::key, schema::Arg<std::string>>,
    schema::Bind<&ZCount::min_str, schema::Arg<std::string>>,
    schema::Bind<&ZCount::max_str, schema::Arg<std::string>>>;

namespace {

std::optional<double> ParseZCountBound(std::string_view s) {
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

io::Reply ExecZCount(engine::Engine& e, const ZCount& x) {
    auto min_v = ParseZCountBound(x.min_str);
    auto max_v = ParseZCountBound(x.max_str);
    if (!min_v || !max_v) {
        return io::MakeErr("ERR min or max is not a float");
    }

    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(static_cast<int64_t>(ptr->sl->CountByScore(*min_v, *max_v)));
}

using ZCountTrait = CommandTrait<"ZCOUNT", ZCountSchema, &ExecZCount>;

}  // namespace valkey::commands
