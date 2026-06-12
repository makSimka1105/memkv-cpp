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

struct LPop {
    std::string key;
    std::optional<int64_t> count;
};

using LPopSchema = schema::Schema<LPop,
    schema::Field<&LPop::key, schema::Arg<std::string>>,
    schema::Field<&LPop::count, schema::Optional<std::optional<int64_t>>>>;

io::Reply ExecLPop(engine::Engine& e, const LPop& x) {
    std::vector<std::string> popped;
    int64_t n = x.count.value_or(1);
    auto st = e.Mutate<types::List>(x.key, [&](auto& lst) -> std::ptrdiff_t {
        n = std::min<int64_t>(n, lst.size());
        std::ptrdiff_t delta = 0;
        for (int64_t i = 0; i < n; ++i) {
            delta -= types::ListTrait::ElementBytes(lst.front());
            popped.push_back(std::move(lst.front()));
            lst.pop_front();
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

using LPopTrait = CommandTrait<"LPOP", LPopSchema, &ExecLPop>;

}  // namespace valkey::commands
