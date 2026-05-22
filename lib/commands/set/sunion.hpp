#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct SUnion {
    std::vector<std::string> keys;
};

using SUnionSchema = schema::Schema<SUnion,
    schema::Bind<&SUnion::keys, schema::Variadic<std::string, 1>>>;

io::Reply ExecSUnion(engine::Engine& e, const SUnion& x) {
    std::unordered_set<std::string> result;
    for (const auto& key : x.keys) {
        auto [s, st] = e.As<std::unordered_set<std::string>>(key);
        if (st == engine::Engine::TypeStatus::kWrongType) {
            return engine::WrongTypeReply();
        }
        if (st == engine::Engine::TypeStatus::kOk) {
            result.insert(s->begin(), s->end());
        }
    }
    std::vector<io::Reply> items;
    items.reserve(result.size());
    for (const auto& m : result) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

using SUnionTrait = CommandTrait<"SUNION", SUnionSchema, &ExecSUnion>;

}  // namespace valkey::commands
