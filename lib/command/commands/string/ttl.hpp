#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"

namespace valkey::commands {

struct Ttl {
    std::string key;
};

using TtlSchema = schema::Schema<Ttl, schema::Field<&Ttl::key, schema::Arg<std::string>>>;

io::Reply ExecTtl(engine::Engine& e, const Ttl& x) {
    return io::MakeInt(e.TtlSeconds(x.key));
}

using TtlTrait = CommandTrait<"TTL", TtlSchema, &ExecTtl>;

}  // namespace valkey::commands
