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

struct Get {
    std::string key;
};

using GetSchema = schema::Schema<Get, schema::Field<&Get::key, schema::Arg<std::string>>>;

io::Reply ExecGet(engine::Engine& e, const Get& x) {
    auto [s, st] = e.As<std::string>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    return io::MakeBulk(*s);
}

using GetTrait = CommandTrait<"GET", GetSchema, &ExecGet>;

}  // namespace valkey::commands
