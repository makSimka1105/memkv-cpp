#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct StrLen {
    std::string key;
};

using StrLenSchema = schema::Schema<StrLen, schema::Bind<&StrLen::key, schema::Arg<std::string>>>;

io::Reply ExecStrLen(engine::Engine& e, const StrLen& x) {
    auto [s, st] = e.As<std::string>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(s->size());
}

using StrLenTrait = CommandTrait<"STRLEN", StrLenSchema, &ExecStrLen>;

}  // namespace valkey::commands
