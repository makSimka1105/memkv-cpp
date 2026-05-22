#pragma once

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct Multi {};

using MultiSchema = schema::Schema<Multi>;

io::Reply ExecMulti(engine::Engine&, const Multi&) {
    return io::MakeErr("ERR command not allowed here");
}

using MultiTrait = CommandTrait<"MULTI", MultiSchema, &ExecMulti>;

}  // namespace valkey::commands
