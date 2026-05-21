#pragma once

#include <string>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"

namespace valkey::cmd_nodes {

struct Get {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::get {


void Build();

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Get& x) {
    auto [s, st] = e.As<std::string>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    return io::MakeBulk(*s);
}

using Trait = meta::CommandTrait<cmd_nodes::Get, "GET", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::get
