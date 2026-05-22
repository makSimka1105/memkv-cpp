#include <sstream>
#include <string>
#include <string_view>

#include "engine/engine.hpp"
#include "io/stdin_frontend.hpp"
#include <gtest/gtest.h>

using namespace valkey;

static std::string Exec(engine::Engine& eng, std::string_view cmds) {
    std::istringstream in{std::string(cmds)};
    std::ostringstream out;
    io::RunStdinRepl(eng, in, out);
    return out.str();
}

TEST(Transaction, MultiExecBasic) {
    engine::Engine eng;
    auto result = Exec(eng, "MULTI\nSET foo bar\nGET foo\nEXEC\n");
    EXPECT_EQ(result, "OK\nQUEUED\nQUEUED\n1) OK\n2) \"bar\"\n");
}

TEST(Transaction, MultiExecEmpty) {
    engine::Engine eng;
    auto result = Exec(eng, "MULTI\nEXEC\n");
    EXPECT_EQ(result, "OK\n(empty array)\n");
}

TEST(Transaction, MultiNested) {
    engine::Engine eng;
    auto result = Exec(eng, "MULTI\nMULTI\n");
    EXPECT_EQ(result, "OK\n(error) ERR MULTI calls can not be nested\n");
}

TEST(Transaction, ExecWithoutMulti) {
    engine::Engine eng;
    auto result = Exec(eng, "EXEC\n");
    EXPECT_EQ(result, "(error) ERR EXEC without MULTI\n");
}

TEST(Transaction, DiscardClearsQueue) {
    engine::Engine eng;
    auto result = Exec(eng, "MULTI\nSET foo bar\nDISCARD\nGET foo\n");
    EXPECT_EQ(result, "OK\nQUEUED\nOK\n(nil)\n");
}

TEST(Transaction, DiscardWithoutMulti) {
    engine::Engine eng;
    auto result = Exec(eng, "DISCARD\n");
    EXPECT_EQ(result, "(error) ERR DISCARD without MULTI\n");
}

TEST(Transaction, MultiExecPersistsToEngine) {
    engine::Engine eng;
    Exec(eng, "SET counter 0\n");
    auto result = Exec(eng, "MULTI\nSET counter 1\nEXEC\n");
    EXPECT_EQ(result, "OK\nQUEUED\n1) OK\n");
    EXPECT_EQ(Exec(eng, "GET counter\n"), "\"1\"\n");
}
