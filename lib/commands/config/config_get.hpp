#pragma once

#include <string>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct ConfigGet {
    std::string param;
};

using ConfigGetSchema = schema::Schema<ConfigGet,
    schema::Bind<&ConfigGet::param, schema::Arg<std::string>>>;

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
