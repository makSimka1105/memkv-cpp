#include <gtest/gtest.h>

#include "commands/dbsize.hpp"
#include "commands/flushdb.hpp"
#include "commands/rpush.hpp"
#include "commands/sadd.hpp"
#include "commands/set.hpp"
#include "commands/type.hpp"
#include "engine/engine.hpp"
#include "engine/interpreter.hpp"
#include "io/reply.hpp"
#include "commands/all.hpp"

using namespace valkey;

TEST(InterpreterGeneric, TypeCommand) {
    auto eng = engine::Engine{};
    EXPECT_EQ(std::get<io::SimpleStr>(engine::Eval(eng, cmd_nodes::Type{"missing"})).value, "none");

    engine::Eval(eng, cmd_nodes::Set{"s", "hello"});
    EXPECT_EQ(std::get<io::SimpleStr>(engine::Eval(eng, cmd_nodes::Type{"s"})).value, "string");

    engine::Eval(eng, cmd_nodes::RPush{"l", {"a"}});
    EXPECT_EQ(std::get<io::SimpleStr>(engine::Eval(eng, cmd_nodes::Type{"l"})).value, "list");

    engine::Eval(eng, cmd_nodes::SAdd{"st", {"x"}});
    EXPECT_EQ(std::get<io::SimpleStr>(engine::Eval(eng, cmd_nodes::Type{"st"})).value, "set");
}

TEST(InterpreterGeneric, DbSizeAndFlushDb) {
    auto eng = engine::Engine{};
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::DBSize{})).value, 0);

    engine::Eval(eng, cmd_nodes::Set{"a", "1"});
    engine::Eval(eng, cmd_nodes::Set{"b", "2"});
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::DBSize{})).value, 2);

    engine::Eval(eng, cmd_nodes::FlushDb{});
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::DBSize{})).value, 0);
}
