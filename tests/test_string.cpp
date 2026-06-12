#include <chrono>
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include "engine/config.hpp"
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

TEST(SetAndGet, BasicRoundtrip) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"SET foo bar\n"), "OK\n");
    EXPECT_EQ(Exec(eng,"GET foo\n"), "\"bar\"\n");
}

TEST(GetMissingKey, ReturnsNil) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"GET nosuchkey\n"), "(nil)\n");
}

TEST(GetWrongType, ReturnsWrongtype) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a\n");
    EXPECT_EQ(Exec(eng,"GET mylist\n"),
              "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
}

TEST(AppendCreatesAndExtends, LengthIsCorrect) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"APPEND greet Hello\n"), "(integer) 5\n");
    EXPECT_EQ(Exec(eng,"APPEND greet World\n"), "(integer) 10\n");
    EXPECT_EQ(Exec(eng,"STRLEN greet\n"), "(integer) 10\n");
}

TEST(AppendWrongType, ReturnsWrongtype) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a\n");
    EXPECT_EQ(Exec(eng,"APPEND mylist x\n"),
              "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
}

TEST(StrlenExisting, ReturnsCorrectLength) {
    engine::Engine eng;
    Exec(eng,"SET word abcde\n");
    EXPECT_EQ(Exec(eng,"STRLEN word\n"), "(integer) 5\n");
}

TEST(StrlenMissingKey, ReturnsZero) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"STRLEN ghost\n"), "(integer) 0\n");
}

TEST(ExpireAndTtl, TtlPositiveAndBounded) {
    engine::Engine eng;
    Exec(eng,"SET k v\n");
    EXPECT_EQ(Exec(eng,"EXPIRE k 10\n"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng,"TTL k\n"), "(integer) 9\n");
    eng.AdvanceClock(std::chrono::seconds(11));
    EXPECT_EQ(Exec(eng,"GET k\n"), "(nil)\n");
    EXPECT_EQ(Exec(eng,"TTL k\n"), "(integer) -2\n");
}

TEST(ExpireAndTtl, ExpiredKeyReleasesMemory) {
    engine::Engine eng;
    const size_t baseline = eng.UsedMemory();
    Exec(eng,"SET k v\n");
    EXPECT_GT(eng.UsedMemory(), baseline);
    EXPECT_EQ(Exec(eng,"EXPIRE k 1\n"), "(integer) 1\n");
    eng.AdvanceClock(std::chrono::seconds(2));
    EXPECT_EQ(Exec(eng,"GET k\n"), "(nil)\n");
    EXPECT_EQ(eng.UsedMemory(), baseline);
    EXPECT_EQ(Exec(eng,"DEL k\n"), "(integer) 0\n");
    EXPECT_EQ(eng.UsedMemory(), baseline);
}

TEST(TtlNoExpiry, ReturnsMinusOne) {
    engine::Engine eng;
    Exec(eng,"SET k v\n");
    EXPECT_EQ(Exec(eng,"TTL k\n"), "(integer) -1\n");
}

TEST(TtlMissingKey, ReturnsMinusTwo) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"TTL ghost\n"), "(integer) -2\n");
}

TEST(SetOom, RejectsWithOomError) {
    engine::Engine eng{engine::Config{.maxmemory = 1}};
    EXPECT_EQ(Exec(eng,"SET k v\n"),
              "(error) OOM command not allowed when used memory > 'maxmemory'\n");
}
