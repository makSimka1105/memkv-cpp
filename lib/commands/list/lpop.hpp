#pragma once
#include <algorithm>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct LPop {
    std::string key;
    std::optional<int64_t> count;
};

using LPopSchema = schema::Schema<LPop,
    schema::Bind<&LPop::key, schema::Arg<std::string>>,
    schema::Bind<&LPop::count, schema::OptArg<std::optional<int64_t>>>>;

io::Reply ExecLPop(engine::Engine& e, const LPop& x) {
    std::vector<std::string> popped;
    int64_t n = x.count.value_or(1);
    auto st = e.Mutate<std::deque<std::string>>(x.key, [&](auto& lst) {
        n = std::min<int64_t>(n, lst.size());
        for (int64_t i = 0; i < n; ++i) {
            popped.push_back(std::move(lst.front()));
            lst.pop_front();
        }
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
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
