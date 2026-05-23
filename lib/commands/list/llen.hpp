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

struct LLen {
    std::string key;
};

using LLenSchema = schema::Schema<LLen, schema::Bind<&LLen::key, schema::Arg<std::string>>>;

io::Reply ExecLLen(engine::Engine& e, const LLen& x) {
    auto [lst, st] = e.As<types::List>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(lst->size());
}

using LLenTrait = CommandTrait<"LLEN", LLenSchema, &ExecLLen>;

}  // namespace valkey::commands
