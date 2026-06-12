#pragma once

#include <string>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct Type {
    std::string key;
};

using TypeSchema = schema::Schema<Type, schema::Field<&Type::key, schema::Arg<std::string>>>;

io::Reply ExecType(engine::Engine& e, const Type& x) {
    return io::SimpleStr{std::string(e.TypeOf(x.key))};
}

using TypeTrait = CommandTrait<"TYPE", TypeSchema, &ExecType>;

}  // namespace valkey::commands
