#pragma once

#include <string>
#include <vector>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct Keys {
    std::string pattern;
};

using KeysSchema = schema::Schema<Keys, schema::Field<&Keys::pattern, schema::Arg<std::string>>>;

io::Reply ExecKeys(engine::Engine& e, const Keys& x) {
    auto keys = e.Keys(x.pattern);
    std::vector<io::Reply> items;
    items.reserve(keys.size());
    for (auto& k : keys) {
        items.push_back(io::MakeBulk(std::move(k)));
    }
    return io::MakeArray(std::move(items));
}

using KeysTrait = CommandTrait<"KEYS", KeysSchema, &ExecKeys>;

}  // namespace valkey::commands
