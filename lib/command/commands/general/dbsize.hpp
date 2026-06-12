#pragma once

#include <cstdint>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct DbSize {};

using DbSizeSchema = schema::Schema<DbSize>;

io::Reply ExecDbSize(engine::Engine& e, const DbSize&) {
    return io::MakeInt(e.DbSize());
}

using DbSizeTrait = CommandTrait<"DBSIZE", DbSizeSchema, &ExecDbSize>;

}  // namespace valkey::commands
