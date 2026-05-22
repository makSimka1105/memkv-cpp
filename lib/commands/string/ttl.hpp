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

struct Ttl {
    std::string key;
};

using TtlSchema = schema::Schema<Ttl, schema::Bind<&Ttl::key, schema::Arg<std::string>>>;

io::Reply ExecTtl(engine::Engine& e, const Ttl& x) {
    return io::MakeInt(e.TtlSeconds(x.key));
}

using TtlTrait = CommandTrait<"TTL", TtlSchema, &ExecTtl>;

}  // namespace valkey::commands
