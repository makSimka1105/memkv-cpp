#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct SCard {
    std::string key;
};

using SCardSchema = schema::Schema<SCard, schema::Bind<&SCard::key, schema::Arg<std::string>>>;

io::Reply ExecSCard(engine::Engine& e, const SCard& x) {
    auto [s, st] = e.As<types::Set>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(s->size());
}

using SCardTrait = CommandTrait<"SCARD", SCardSchema, &ExecSCard>;

}  // namespace valkey::commands
