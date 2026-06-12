#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "command/commands/script/eval.hpp"
#include "command/commands/script/func.hpp"
#include "command/commands/script/funccall.hpp"
#include "engine/engine.hpp"
#include "engine/eval.hpp"
#include "io/human.hpp"
#include "io/reply.hpp"

using namespace valkey;

namespace {

std::vector<std::string> KeysThenArgv(const std::vector<std::string>& keys,
                                      const std::vector<std::string>& argv) {
    std::vector<std::string> rest = keys;
    rest.insert(rest.end(), argv.begin(), argv.end());
    return rest;
}

std::string ToText(const io::Reply& reply) {
    std::ostringstream out;
    io::human::Write(out, reply);
    return out.str();
}

class ScriptFunctions : public ::testing::Test {
protected:
    engine::Engine eng_;

    void SetUp() override { engine::SetupEngine(eng_); }

    std::string Eval(const std::string& body, std::vector<std::string> keys,
                     std::vector<std::string> argv) {
        commands::Eval cmd{body, static_cast<int64_t>(keys.size()), KeysThenArgv(keys, argv)};
        return ToText(commands::ExecEval(eng_, cmd));
    }

    void Register(const std::string& name, const std::string& body) {
        commands::ExecFunc(eng_, commands::Func{name, body});
    }

    std::string Call(const std::string& name, std::vector<std::string> keys,
                     std::vector<std::string> argv) {
        commands::FuncCall cmd{name, static_cast<int64_t>(keys.size()), KeysThenArgv(keys, argv)};
        return ToText(commands::ExecFuncCall(eng_, cmd));
    }
};

TEST_F(ScriptFunctions, EvalExercisesCoreMechanics) {
    const std::string script = R"(
        let total = 0;
        for word in argv {
            call("RPUSH", keys[0], word);
            total = total + len(word);
        }
        if (total > 5) {
            call("SET", keys[1], "big");
        } else {
            call("SET", keys[1], "small");
        }
        return [call("LLEN", keys[0]), total, call("GET", keys[1])];
    )";

    auto out = Eval(script, {"mylist", "flag"}, {"aa", "bbb", "c"});

    EXPECT_EQ(out,
              "1) (integer) 3\n"
              "2) (integer) 6\n"
              "3) \"big\"\n");
}

TEST_F(ScriptFunctions, FuncRegisterBigScriptThenCallTwice) {
    Register("summarize", R"(
        let i = 0;
        let even = 0;
        let total = 0;
        while (i < len(argv)) {
            let length = len(argv[i]);
            total = total + length;
            if ((length % 2) == 0) {
                even = even + 1;
            }
            i = i + 1;
        }
        call("SET", keys[0], total);
        call("RPUSH", keys[1], "tick");
        let calls = call("LLEN", keys[1]);
        let big = (total >= 5) and (even > 0);
        return [calls, even, total, big];
    )");

    auto first = Call("summarize", {"sum", "log"}, {"aa", "bbb", "cccc"});
    auto second = Call("summarize", {"sum", "log"}, {"x", "yy"});

    EXPECT_EQ(first,
              "1) (integer) 1\n"
              "2) (integer) 2\n"
              "3) (integer) 9\n"
              "4) (true)\n");
    EXPECT_EQ(second,
              "1) (integer) 2\n"
              "2) (integer) 1\n"
              "3) (integer) 3\n"
              "4) (false)\n");
}

TEST_F(ScriptFunctions, NestedFuncCallThroughFuncCall) {
    Register("addtag", R"(
        call("SADD", keys[0], argv[0]);
        return call("SCARD", keys[0]);
    )");

    Register("tagall", R"(
        let last = 0;
        for tag in argv {
            last = call("FUNCCALL", "addtag", "1", keys[0], tag);
        }
        return last;
    )");

    auto first = Call("tagall", {"tags"}, {"red", "green", "red", "blue"});
    auto second = Call("tagall", {"tags"}, {"red", "yellow"});

    EXPECT_EQ(first, "(integer) 3\n");
    EXPECT_EQ(second, "(integer) 4\n");
}

TEST_F(ScriptFunctions, DirectCallToRegisteredFunctionByAtSyntax) {
    Register("double_len", R"(
        let t = 0;
        for a in argv {
            t = t + len(a);
        }
        return t * 2;
    )");

    auto out = Eval(R"(return @double_len("aa", "bbb");)", {}, {});
    EXPECT_EQ(out, "(integer) 10\n");
}

}  // namespace
