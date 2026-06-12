#pragma once

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct Discard {};

using DiscardSchema = schema::Schema<Discard>;

io::Reply ExecDiscard(engine::Engine&, const Discard&) {
    return io::MakeErr("ERR command not allowed here");
}

using DiscardTrait = CommandTrait<"DISCARD", DiscardSchema, &ExecDiscard>;

}  // namespace valkey::commands
