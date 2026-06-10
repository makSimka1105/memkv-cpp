#pragma once

#include <span>
#include <string>
#include <utility>
#include <variant>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "script/ast.hpp"
#include "script/box.hpp"
#include "script/evaluator/core/context.hpp"
#include "script/evaluator/core/convert.hpp"
#include "script/evaluator/core/dispatch.hpp"
#include "script/evaluator/core/env.hpp"
#include "script/evaluator/expr/bin_op.hpp"
#include "script/evaluator/expr/call.hpp"
#include "script/evaluator/expr/ident.hpp"
#include "script/evaluator/expr/index.hpp"
#include "script/evaluator/expr/list_lit.hpp"
#include "script/evaluator/expr/literals.hpp"
#include "script/evaluator/expr/script_call.hpp"
#include "script/evaluator/expr/un_op.hpp"
#include "script/evaluator/stmt/assign.hpp"
#include "script/evaluator/stmt/block.hpp"
#include "script/evaluator/stmt/expr_stmt.hpp"
#include "script/evaluator/stmt/for.hpp"
#include "script/evaluator/stmt/if.hpp"
#include "script/evaluator/stmt/let.hpp"
#include "script/evaluator/stmt/return.hpp"
#include "script/evaluator/stmt/while.hpp"
#include "script/value.hpp"

namespace valkey::script {

void BindKeysArgv(Env& env, std::span<const std::string> keys, std::span<const std::string> argv) {
    auto to_list = [](std::span<const std::string> values) {
        List list;
        list.items.reserve(values.size());
        for (const auto& value : values) {
            list.items.push_back(ScriptValue{value});
        }
        return ScriptValue{Box<List>{std::move(list)}};
    };
    env.Declare("keys", to_list(keys));
    env.Declare("argv", to_list(argv));
}

io::Reply Execute(engine::Engine& engine, const Program& program, std::span<const std::string> keys,
                  std::span<const std::string> argv) {
    Env env;
    BindKeysArgv(env, keys, argv);
    EvalContext ctx{
        .env = env, .engine = engine, .steps = 0, .step_limit = engine.ScriptMaxIters()};
    for (const auto& statement : program.stmts) {
        auto flow = Exec(statement, ctx);
        if (!flow) {
            return io::MakeErr("ERR " + flow.error().message);
        }
        if (const auto* returned = std::get_if<Return>(&*flow)) {
            return ToReply(returned->value);
        }
    }
    return io::MakeNil();
}

}  // namespace valkey::script
