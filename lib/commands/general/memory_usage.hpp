#pragma once

#include <cstdint>
#include <string>

#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/schema.hpp"

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
