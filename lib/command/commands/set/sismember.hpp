#pragma once
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"

namespace valkey::commands {

struct SIsMember {
    std::string key;
    std::string member;
};

using SIsMemberSchema = schema::Schema<SIsMember,
    schema::Field<&SIsMember::key, schema::Arg<std::string>>,
    schema::Field<&SIsMember::member, schema::Arg<std::string>>>;

io::Reply ExecSIsMember(engine::Engine& e, const SIsMember& x) {
    auto [s, st] = e.As<types::Set>(x.key);
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
