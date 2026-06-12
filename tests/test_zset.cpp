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

TEST(ZSet, ZAddOrderingAndScoreUpdate) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "ZADD board 2 b 1 a 3 c\n"), "(integer) 3\n");
    EXPECT_EQ(Exec(eng, "ZRANGE board 0 -1 WITHSCORES\n"),
              "1) \"a\"\n"
              "2) \"1\"\n"
              "3) \"b\"\n"
              "4) \"2\"\n"
              "5) \"c\"\n"
              "6) \"3\"\n");
    EXPECT_EQ(Exec(eng, "ZADD board 5 a\n"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "ZRANGE board 0 -1\n"),
              "1) \"b\"\n"
              "2) \"c\"\n"
              "3) \"a\"\n");
}

TEST(ZSet, EqualScoreLexOrderAndRank) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "ZADD t 1 bb 1 aa 1 cc\n"), "(integer) 3\n");
    EXPECT_EQ(Exec(eng, "ZRANGE t 0 -1\n"),
              "1) \"aa\"\n"
              "2) \"bb\"\n"
              "3) \"cc\"\n");
    EXPECT_EQ(Exec(eng, "ZRANK t cc\n"), "(integer) 2\n");
    EXPECT_EQ(Exec(eng, "ZRANK t ghost\n"), "(nil)\n");
    EXPECT_EQ(Exec(eng, "ZSCORE t bb\n"), "\"1\"\n");
    EXPECT_EQ(Exec(eng, "ZSCORE nokey x\n"), "(nil)\n");
}
