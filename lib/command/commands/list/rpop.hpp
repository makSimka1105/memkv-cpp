#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
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

struct RPop {
    std::string key;
    std::optional<int64_t> count;
};

using RPopSchema = schema::Schema<RPop,
    schema::Field<&RPop::key, schema::Arg<std::string>>,
    schema::Field<&RPop::count, schema::Optional<std::optional<int64_t>>>>;

io::Reply ExecRPop(engine::Engine& e, const RPop& x) {
    std::vector<std::string> popped;
    int64_t n = x.count.value_or(1);
    auto st = e.Mutate<types::List>(x.key, [&](auto& lst) -> std::ptrdiff_t {
        n = std::min<int64_t>(n, lst.size());
        std::ptrdiff_t delta = 0;
        for (int64_t i = 0; i < n; ++i) {
            delta -= types::ListTrait::ElementBytes(lst.back());
            popped.push_back(std::move(lst.back()));
            lst.pop_back();
        }
        return delta;
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return x.count.has_value() ? io::MakeArray({}) : io::MakeNil();
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (!x.count.has_value()) {
        return popped.empty() ? io::MakeNil() : io::MakeBulk(std::move(popped[0]));
    }
    std::vector<io::Reply> items;
    items.reserve(popped.size());
    for (auto& s : popped) {
        items.push_back(io::MakeBulk(std::move(s)));
    }
    return io::MakeArray(std::move(items));
}

using RPopTrait = CommandTrait<"RPOP", RPopSchema, &ExecRPop>;

}  // namespace valkey::commands
