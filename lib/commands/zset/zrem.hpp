#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct ZRem {
    std::string key;
    std::vector<std::string> members;
};

using ZRemSchema = schema::Schema<ZRem,
    schema::Bind<&ZRem::key, schema::Arg<std::string>>,
    schema::Bind<&ZRem::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecZRem(engine::Engine& e, const ZRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<types::ZSet>(x.key, [&](auto& z) {
        for (const auto& m : x.members) {
            auto it = z.scores.find(m);
            if (it == z.scores.end()) {
                continue;
            }
            z.sl->Erase(it->second, m);
            z.scores.erase(it);
            ++removed;
        }
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

using ZRemTrait = CommandTrait<"ZREM", ZRemSchema, &ExecZRem>;

}  // namespace valkey::commands
