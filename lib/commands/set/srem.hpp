#pragma once
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct SRem {
    std::string key;
    std::vector<std::string> members;
};

using SRemSchema = schema::Schema<SRem,
    schema::Bind<&SRem::key, schema::Arg<std::string>>,
    schema::Bind<&SRem::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecSRem(engine::Engine& e, const SRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<std::unordered_set<std::string>>(x.key, [&](auto& s) {
        for (const auto& m : x.members) {
            if (s.erase(m)) {
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

using SRemTrait = CommandTrait<"SREM", SRemSchema, &ExecSRem>;

}  // namespace valkey::commands
