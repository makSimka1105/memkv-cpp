#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/variadic.hpp"

namespace valkey::commands {

struct ZRem {
    std::string key;
    std::vector<std::string> members;
};

using ZRemSchema = schema::Schema<ZRem,
    schema::Field<&ZRem::key, schema::Arg<std::string>>,
    schema::Field<&ZRem::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecZRem(engine::Engine& e, const ZRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<types::ZSet>(x.key, [&](auto& z) {
        for (const auto& m : x.members) {
            if (z.RemoveMember(m)) {
                ++removed;
            }
        }
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

using ZRemTrait = CommandTrait<"ZREM", ZRemSchema, &ExecZRem>;

}  // namespace valkey::commands
