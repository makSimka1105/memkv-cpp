#pragma once

#include <string>

#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/schema.hpp"

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
