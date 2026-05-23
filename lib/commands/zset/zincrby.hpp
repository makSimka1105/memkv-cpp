#pragma once
#include <format>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct ZIncrBy {
    std::string key;
    double increment;
    std::string member;
};

using ZIncrBySchema = schema::Schema<ZIncrBy,
    schema::Bind<&ZIncrBy::key, schema::Arg<std::string>>,
    schema::Bind<&ZIncrBy::increment, schema::Arg<double>>,
    schema::Bind<&ZIncrBy::member, schema::Arg<std::string>>>;

io::Reply ExecZIncrBy(engine::Engine& e, const ZIncrBy& x) {
    double new_score = 0.0;
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) {
        auto it = z.scores.find(x.member);
        if (it != z.scores.end()) {
            z.sl->Erase(it->second, x.member);
            it->second += x.increment;
            new_score = it->second;
        } else {
            z.scores[x.member] = x.increment;
            new_score = x.increment;
        }
        z.sl->Insert(new_score, x.member);
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeBulk(std::format("{:.17g}", new_score));
}

using ZIncrByTrait = CommandTrait<"ZINCRBY", ZIncrBySchema, &ExecZIncrBy>;

}  // namespace valkey::commands
