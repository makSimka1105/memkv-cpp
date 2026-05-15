#include <memory>
#include <variant>
#include <vector>

#include <gtest/gtest.h>

#include "engine/engine.hpp"
#include "engine/interpreter.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"

using namespace valkey;

TEST(InterpreterSet, SAddSMembersSCard) {
    auto eng = engine::Engine{};
    auto r = engine::Eval(eng, cmd_nodes::SAdd{"s", {"a", "b", "c", "a"}});
    EXPECT_EQ(std::get<io::Int>(r).value, 3);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::SCard{"s"})).value, 3);

    auto mem = engine::Eval(eng, cmd_nodes::SMembers{"s"});
    const auto& arr = std::get<std::unique_ptr<io::Array>>(mem);
    EXPECT_EQ(arr->items.size(), 3);
}

TEST(InterpreterSet, SRemSIsMember) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::SAdd{"s", {"a", "b", "c"}});

    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::SIsMember{"s", "b"})).value, 1);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::SIsMember{"s", "z"})).value, 0);

    auto r = engine::Eval(eng, cmd_nodes::SRem{"s", {"a", "z"}});
    EXPECT_EQ(std::get<io::Int>(r).value, 1);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::SCard{"s"})).value, 2);
}

TEST(InterpreterSet, SetOps) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::SAdd{"a", {"1", "2", "3"}});
    engine::Eval(eng, cmd_nodes::SAdd{"b", {"2", "3", "4"}});

    auto u = engine::Eval(eng, cmd_nodes::SUnion{{"a", "b"}});
    EXPECT_EQ(std::get<std::unique_ptr<io::Array>>(u)->items.size(), 4);

    auto i = engine::Eval(eng, cmd_nodes::SInter{{"a", "b"}});
    EXPECT_EQ(std::get<std::unique_ptr<io::Array>>(i)->items.size(), 2);

    auto d = engine::Eval(eng, cmd_nodes::SDiff{{"a", "b"}});
    EXPECT_EQ(std::get<std::unique_ptr<io::Array>>(d)->items.size(), 1);
}

TEST(InterpreterSet, WrongType) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::Set{"k", "val"});
    EXPECT_TRUE(std::holds_alternative<io::Err>(engine::Eval(eng, cmd_nodes::SAdd{"k", {"x"}})));
    EXPECT_TRUE(std::holds_alternative<io::Err>(engine::Eval(eng, cmd_nodes::SCard{"k"})));
}
