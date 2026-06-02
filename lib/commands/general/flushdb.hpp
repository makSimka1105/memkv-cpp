#pragma once

#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct FlushDb {};

using FlushDbSchema = schema::Schema<FlushDb>;

io::Reply ExecFlushDb(engine::Engine& e, const FlushDb&) {
    e.FlushDb();
    return io::Ok();
}

using FlushDbTrait = CommandTrait<"FLUSHDB", FlushDbSchema, &ExecFlushDb>;

}  // namespace valkey::commands
