#pragma once
#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/optional.hpp"

namespace valkey::commands {

struct ZPopMin {
    std::string key;
    std::optional<int64_t> count;
};

using ZPopMinSchema = schema::Schema<ZPopMin,
    schema::Field<&ZPopMin::key, schema::Arg<std::string>>,
    schema::Field<&ZPopMin::count, schema::Optional<std::optional<int64_t>>>>;

io::Reply ExecZPopMin(engine::Engine& e, const ZPopMin& x) {
    size_t n = x.count.value_or(1);
    std::vector<io::Reply> items;
    auto st = e.Mutate<types::ZSet>(x.key, [&](auto& z) -> std::ptrdiff_t {
        auto popped = z.PopMin(n);
        items.reserve(popped.size() * 2);
        std::ptrdiff_t delta = 0;
        for (const auto& [score, member] : popped) {
            delta -= types::ZSetTrait::ElementBytes(member);
            items.push_back(io::MakeBulk(member));
            items.push_back(io::MakeBulk(std::format("{:.17g}", score)));
        }
        return delta;
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeArray(std::move(items));
}

using ZPopMinTrait = CommandTrait<"ZPOPMIN", ZPopMinSchema, &ExecZPopMin>;

}  // namespace valkey::commands
