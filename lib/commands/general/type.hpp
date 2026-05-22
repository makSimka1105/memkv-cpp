#pragma once

#include <string>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct Type {
    std::string key;
};

using TypeSchema = schema::Schema<Type, schema::Bind<&Type::key, schema::Arg<std::string>>>;

io::Reply ExecType(engine::Engine& e, const Type& x) {
    return io::SimpleStr{std::string(e.TypeOf(x.key))};
}

using TypeTrait = CommandTrait<"TYPE", TypeSchema, &ExecType>;

}  // namespace valkey::commands
