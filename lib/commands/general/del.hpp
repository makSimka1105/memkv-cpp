#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct Del {
    std::vector<std::string> keys;
};

using DelSchema = schema::Schema<Del,
    schema::Bind<&Del::keys, schema::Variadic<std::string, 1>>>;

io::Reply ExecDel(engine::Engine& e, const Del& x) {
    int64_t count = 0;
    for (const auto& key : x.keys) {
        if (e.DelKey(key)) {
            ++count;
        }
    }
    return io::MakeInt(count);
}

using DelTrait = CommandTrait<"DEL", DelSchema, &ExecDel>;

}  // namespace valkey::commands
