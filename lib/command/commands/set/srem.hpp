#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/variadic.hpp"

namespace valkey::commands {

struct SRem {
    std::string key;
    std::vector<std::string> members;
};

using SRemSchema = schema::Schema<SRem,
    schema::Field<&SRem::key, schema::Arg<std::string>>,
    schema::Field<&SRem::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecSRem(engine::Engine& e, const SRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<types::Set>(x.key, [&](auto& s) -> std::ptrdiff_t {
        std::ptrdiff_t delta = 0;
        for (const auto& m : x.members) {
            if (s.erase(m)) {
                ++removed;
                delta -= types::SetTrait::ElementBytes(m);
            }
        }
        return delta;
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

using SRemTrait = CommandTrait<"SREM", SRemSchema, &ExecSRem>;

}  // namespace valkey::commands
