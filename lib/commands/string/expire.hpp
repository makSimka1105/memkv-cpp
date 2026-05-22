#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct Expire {
    std::string key;
    int64_t seconds;
};

using ExpireSchema = schema::Schema<Expire,
    schema::Bind<&Expire::key, schema::Arg<std::string>>,
    schema::Bind<&Expire::seconds, schema::Arg<int64_t>>>;

io::Reply ExecExpire(engine::Engine& e, const Expire& x) {
    bool ok = e.ExpireKey(x.key, std::chrono::seconds(x.seconds));
    return io::MakeInt(ok ? 1 : 0);
}

using ExpireTrait = CommandTrait<"EXPIRE", ExpireSchema, &ExecExpire>;

}  // namespace valkey::commands
