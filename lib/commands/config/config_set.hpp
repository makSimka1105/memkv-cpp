#pragma once

#include <string>

#include "engine/config.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct ConfigSet {
    std::string param;
    std::string value;
};

using ConfigSetSchema = schema::Schema<ConfigSet,
    schema::Bind<&ConfigSet::param, schema::Arg<std::string>>,
    schema::Bind<&ConfigSet::value, schema::Arg<std::string>>>;

io::Reply ExecConfigSet(engine::Engine& e, const ConfigSet& x) {
    if (x.param == "maxmemory") {
        auto n = engine::ParseMemorySize(x.value);
        if (!n) {
            return io::MakeErr("ERR Invalid argument '" + x.value +
                               "' for CONFIG SET 'maxmemory'");
        }
        e.SetMaxMemory(*n);
        return io::Ok();
    }
    return io::MakeErr("ERR Unknown option or number of arguments for CONFIG SET - '" +
                       x.param + "'");
}

using ConfigSetTrait = CommandTrait<"SET", ConfigSetSchema, &ExecConfigSet>;

}  // namespace valkey::commands
