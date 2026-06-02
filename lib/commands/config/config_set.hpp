#pragma once

#include <string>

#include "commands/tree.hpp"
#include "engine/config.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct ConfigSet {
    std::string param;
    std::string value;
};

using ConfigSetSchema = schema::Schema<ConfigSet,
    schema::Field<&ConfigSet::param, schema::Arg<std::string>>,
    schema::Field<&ConfigSet::value, schema::Arg<std::string>>>;

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
