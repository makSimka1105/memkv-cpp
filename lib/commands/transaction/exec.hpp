#pragma once

#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct Exec {};

using ExecSchema = schema::Schema<Exec>;

io::Reply ExecExec(engine::Engine&, const Exec&) {
    return io::MakeErr("ERR command not allowed here");
}

using ExecTrait = CommandTrait<"EXEC", ExecSchema, &ExecExec>;

}  // namespace valkey::commands
