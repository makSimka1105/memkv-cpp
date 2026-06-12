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

struct SMove {
    std::string source;
    std::string dest;
    std::string member;
};

using SMoveSchema = schema::Schema<SMove,
    schema::Field<&SMove::source, schema::Arg<std::string>>,
    schema::Field<&SMove::dest, schema::Arg<std::string>>,
    schema::Field<&SMove::member, schema::Arg<std::string>>>;

io::Reply ExecSMove(engine::Engine& e, const SMove& x) {
    bool removed = false;
    auto st = e.Mutate<types::Set>(x.source, [&](auto& s) {
        removed = s.erase(x.member) > 0;
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (!removed) {
        return io::MakeInt(0);
    }
    auto upsert_st = e.Upsert<types::Set>(x.dest, [&](auto& s) {
        s.insert(x.member);
    });
    if (upsert_st == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (upsert_st == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeInt(1);
}

using SMoveTrait = CommandTrait<"SMOVE", SMoveSchema, &ExecSMove>;

}  // namespace valkey::commands
