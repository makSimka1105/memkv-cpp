#pragma once
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct SMembers {
    std::string key;
};

using SMembersSchema = schema::Schema<SMembers,
    schema::Bind<&SMembers::key, schema::Arg<std::string>>>;

io::Reply ExecSMembers(engine::Engine& e, const SMembers& x) {
    auto [s, st] = e.As<types::Set>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    std::vector<io::Reply> items;
    items.reserve(s->size());
    for (const auto& m : *s) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

using SMembersTrait = CommandTrait<"SMEMBERS", SMembersSchema, &ExecSMembers>;

}  // namespace valkey::commands
