#pragma once
#include <string>
#include <unordered_set>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct SIsMember {
    std::string key;
    std::string member;
};

using SIsMemberSchema = schema::Schema<SIsMember,
    schema::Bind<&SIsMember::key, schema::Arg<std::string>>,
    schema::Bind<&SIsMember::member, schema::Arg<std::string>>>;

io::Reply ExecSIsMember(engine::Engine& e, const SIsMember& x) {
    auto [s, st] = e.As<std::unordered_set<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(s->count(x.member) ? 1 : 0);
}

using SIsMemberTrait = CommandTrait<"SISMEMBER", SIsMemberSchema, &ExecSIsMember>;

}  // namespace valkey::commands
