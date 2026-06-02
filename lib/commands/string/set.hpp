#pragma once

#include <string>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"

namespace valkey::commands {

struct Set {
    std::string key;
    std::string value;
};

using SetSchema = schema::Schema<Set, schema::Field<&Set::key, schema::Arg<std::string>>,
                                 schema::Field<&Set::value, schema::Arg<std::string>>>;

io::Reply ExecSet(engine::Engine& e, const Set& x) {
    if (!e.SetString(x.key, x.value)) {
        return engine::OomReply();
    }
    return io::Ok();
}

using SetTrait = CommandTrait<"SET", SetSchema, &ExecSet>;

}  // namespace valkey::commands
