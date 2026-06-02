#pragma once
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/optional.hpp"

namespace valkey::commands {

struct ZPopMax {
    std::string key;
    std::optional<int64_t> count;
};

using ZPopMaxSchema = schema::Schema<ZPopMax,
    schema::Field<&ZPopMax::key, schema::Arg<std::string>>,
    schema::Field<&ZPopMax::count, schema::Optional<std::optional<int64_t>>>>;

io::Reply ExecZPopMax(engine::Engine& e, const ZPopMax& x) {
    size_t n = x.count.value_or(1);
    std::vector<io::Reply> items;
    auto st = e.Mutate<types::ZSet>(x.key, [&](auto& z) {
        auto popped = z.sl->PopMax(n);
        items.reserve(popped.size() * 2);
        for (const auto& [score, member] : popped) {
            z.scores.erase(member);
            items.push_back(io::MakeBulk(member));
            items.push_back(io::MakeBulk(std::format("{:.17g}", score)));
        }
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeArray(std::move(items));
}

using ZPopMaxTrait = CommandTrait<"ZPOPMAX", ZPopMaxSchema, &ExecZPopMax>;

}  // namespace valkey::commands
