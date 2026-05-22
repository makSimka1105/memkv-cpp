#pragma once

#include <string>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct Set {
    std::string key;
    std::string value;
};

using SetSchema = schema::Schema<Set, schema::Bind<&Set::key, schema::Arg<std::string>>,
                                 schema::Bind<&Set::value, schema::Arg<std::string>>>;

io::Reply ExecSet(engine::Engine& e, const Set& x) {
    if (!e.SetString(x.key, x.value)) {
        return engine::OomReply();
    }
    return io::Ok();
}

using SetTrait = CommandTrait<"SET", SetSchema, &ExecSet>;

}  // namespace valkey::commands
