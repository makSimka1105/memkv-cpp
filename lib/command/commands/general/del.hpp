#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/variadic.hpp"
#include "command/schema/schema.hpp"

namespace valkey::commands {

struct Del {
    std::vector<std::string> keys;
};

using DelSchema = schema::Schema<Del,
    schema::Field<&Del::keys, schema::Variadic<std::string, 1>>>;

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
