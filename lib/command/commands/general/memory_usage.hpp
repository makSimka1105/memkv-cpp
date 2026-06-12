#pragma once

#include <cstdint>
#include <string>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct MemoryUsage {
    std::string key;
};

using MemoryUsageSchema = schema::Schema<MemoryUsage,
    schema::Field<&MemoryUsage::key, schema::Arg<std::string>>>;

io::Reply ExecMemoryUsage(engine::Engine& e, const MemoryUsage& x) {
    return io::MakeInt(e.MemoryUsage(x.key));
}

using MemoryUsageTrait = CommandTrait<"USAGE", MemoryUsageSchema, &ExecMemoryUsage>;

}  // namespace valkey::commands
