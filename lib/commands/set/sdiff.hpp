#pragma once
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct SDiff {
    std::vector<std::string> keys;
};

using SDiffSchema = schema::Schema<SDiff,
    schema::Bind<&SDiff::keys, schema::Variadic<std::string, 1>>>;

io::Reply ExecSDiff(engine::Engine& e, const SDiff& x) {
    auto [first, st0] = e.As<types::Set>(x.keys[0]);
    if (st0 == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st0 == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    types::Set result(*first);
    for (size_t i = 1; i < x.keys.size(); ++i) {
        auto [s, st] = e.As<types::Set>(x.keys[i]);
        if (st == engine::Engine::TypeStatus::kWrongType) {
            return engine::WrongTypeReply();
        }
        if (st == engine::Engine::TypeStatus::kOk) {
            for (const auto& m : *s) {
                result.erase(m);
            }
        }
    }
    std::vector<io::Reply> items;
    items.reserve(result.size());
    for (const auto& m : result) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

using SDiffTrait = CommandTrait<"SDIFF", SDiffSchema, &ExecSDiff>;

}  // namespace valkey::commands
