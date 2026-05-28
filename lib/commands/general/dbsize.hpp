#pragma once

#include <cstdint>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct DbSize {};

using DbSizeSchema = schema::Schema<DbSize>;

io::Reply ExecDbSize(engine::Engine& e, const DbSize&) {
    return io::MakeInt(e.DbSize());
}

using DbSizeTrait = CommandTrait<"DBSIZE", DbSizeSchema, &ExecDbSize>;

}  // namespace valkey::commands
