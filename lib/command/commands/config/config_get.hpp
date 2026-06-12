#pragma once

#include <string>
#include <utility>
#include <vector>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct ConfigGet {
    std::string param;
};

using ConfigGetSchema = schema::Schema<ConfigGet,
    schema::Field<&ConfigGet::param, schema::Arg<std::string>>>;

io::Reply ExecConfigGet(engine::Engine& e, const ConfigGet& x) {
    std::vector<io::Reply> items;
    items.push_back(io::MakeBulk(x.param));
    if (x.param == "maxmemory") {
        items.push_back(io::MakeBulk(std::to_string(e.MaxMemory())));
    } else {
        items.push_back(io::MakeNil());
    }
    return io::MakeArray(std::move(items));
}

using ConfigGetTrait = CommandTrait<"GET", ConfigGetSchema, &ExecConfigGet>;

}  // namespace valkey::commands
