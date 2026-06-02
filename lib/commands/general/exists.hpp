#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/variadic.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct Exists {
    std::vector<std::string> keys;
};

using ExistsSchema = schema::Schema<Exists,
    schema::Field<&Exists::keys, schema::Variadic<std::string, 1>>>;

io::Reply ExecExists(engine::Engine& e, const Exists& x) {
    int64_t count = 0;
    for (const auto& key : x.keys) {
        if (e.ExistsKey(key)) {
            ++count;
        }
    }
    return io::MakeInt(count);
}

using ExistsTrait = CommandTrait<"EXISTS", ExistsSchema, &ExecExists>;

}  // namespace valkey::commands
