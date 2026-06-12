#pragma once

#include <string>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct Set {
    std::string key;
    std::string value;
};

using SetSchema = schema::Schema<Set, schema::Field<&Set::key, schema::Arg<std::string>>,
                                 schema::Field<&Set::value, schema::Arg<std::string>>>;

io::Reply ExecSet(engine::Engine& e, const Set& x) {
    if (!e.ForceSet<types::String>(x.key, x.value)) {
        return engine::OomReply();
    }
    return io::Ok();
}

using SetTrait = CommandTrait<"SET", SetSchema, &ExecSet>;

}  // namespace valkey::commands
