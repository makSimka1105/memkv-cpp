#pragma once

#include <cstdint>
#include <string>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct MemoryUsage {
    std::string key;
};

using MemoryUsageSchema = schema::Schema<MemoryUsage,
    schema::Bind<&MemoryUsage::key, schema::Arg<std::string>>>;

io::Reply ExecMemoryUsage(engine::Engine& e, const MemoryUsage& x) {
    return io::MakeInt(e.MemoryUsage(x.key));
}

using MemoryUsageTrait = CommandTrait<"USAGE", MemoryUsageSchema, &ExecMemoryUsage>;

}  // namespace valkey::commands
