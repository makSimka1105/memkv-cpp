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
    EXPECT_EQ(Exec(eng,"APPEND greet Hello\n"), "(int) 5\n");
    EXPECT_EQ(Exec(eng,"APPEND greet World\n"), "(int) 10\n");
    EXPECT_EQ(Exec(eng,"STRLEN greet\n"), "(int) 10\n");
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
    EXPECT_EQ(Exec(eng,"STRLEN word\n"), "(int) 5\n");
}

TEST(StrlenMissingKey, ReturnsZero) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"STRLEN ghost\n"), "(int) 0\n");
}

TEST(ExpireAndTtl, TtlPositiveAndBounded) {
    engine::Engine eng;
    Exec(eng,"SET k v\n");
    EXPECT_EQ(Exec(eng,"EXPIRE k 10\n"), "(int) 1\n");
    std::istringstream in{"TTL k\n"};
    std::ostringstream out;
    io::RunStdinRepl(eng, in, out);
    const std::string result = out.str();
    ASSERT_EQ(result.substr(0, 6), "(int) ");
    int ttl = std::stoi(result.substr(6));
    EXPECT_GT(ttl, 0);
    EXPECT_LE(ttl, 10);
}

TEST(TtlNoExpiry, ReturnsMinusOne) {
    engine::Engine eng;
    Exec(eng,"SET k v\n");
    EXPECT_EQ(Exec(eng,"TTL k\n"), "(int) -1\n");
}

TEST(TtlMissingKey, ReturnsMinusTwo) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"TTL ghost\n"), "(int) -2\n");
}

TEST(SetOom, RejectsWithOomError) {
    engine::Engine eng{engine::Config{.maxmemory = 1}};
    EXPECT_EQ(Exec(eng,"SET k v\n"),
              "(error) OOM command not allowed when used memory > 'maxmemory'\n");
}
