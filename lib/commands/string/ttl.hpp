#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"

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
