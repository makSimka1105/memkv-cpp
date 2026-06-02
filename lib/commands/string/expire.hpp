#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"

namespace valkey::commands {

struct Expire {
    std::string key;
    int64_t seconds;
};

using ExpireSchema = schema::Schema<Expire,
    schema::Field<&Expire::key, schema::Arg<std::string>>,
    schema::Field<&Expire::seconds, schema::Arg<int64_t>>>;

io::Reply ExecExpire(engine::Engine& e, const Expire& x) {
    bool ok = e.ExpireKey(x.key, std::chrono::seconds(x.seconds));
    return io::MakeInt(ok ? 1 : 0);
}

using ExpireTrait = CommandTrait<"EXPIRE", ExpireSchema, &ExecExpire>;

}  // namespace valkey::commands
