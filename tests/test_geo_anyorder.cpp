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

TEST(GeoSearchAnyOrder, OptionOrderIsEquivalent) {
    engine::Engine eng;
    std::string a = Exec(eng,
        "GEOSEARCH nokey FROMLONLAT 0 0 BYRADIUS 100 m WITHDIST COUNT 5\n");
    std::string b = Exec(eng,
        "GEOSEARCH nokey FROMLONLAT 0 0 BYRADIUS 100 m COUNT 5 WITHDIST\n");
    EXPECT_EQ(a, b);
    EXPECT_EQ(a, "(empty array)\n");
}

TEST(GeoSearchAnyOrder, UnknownTrailingOptionIsSyntaxError) {
    engine::Engine eng;
    std::string r = Exec(eng,
        "GEOSEARCH nokey FROMLONLAT 0 0 BYRADIUS 100 m BOGUS\n");
    EXPECT_EQ(r, "(error) ERR syntax error\n");
}
