#pragma once

#include <utility>
#include <variant>
#include <vector>

#include "commands/transaction/discard.hpp"
#include "commands/transaction/exec.hpp"
#include "commands/transaction/multi.hpp"
#include "engine/engine.hpp"
#include "engine/eval.hpp"
#include "engine/session.hpp"
#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "parser/cmd.hpp"

namespace valkey::engine {

io::Reply SessionEval(Engine& eng, Session& session, const parser::Cmd& cmd) {
    return std::visit(
        Overload{
            [&](const commands::Multi&) -> io::Reply {
                if (session.in_multi) {
                    return io::MakeErr("ERR MULTI calls can not be nested");
                }
                session.in_multi = true;
                return io::Ok();
            },
            [&](const commands::Exec&) -> io::Reply {
                if (!session.in_multi) {
                    return io::MakeErr("ERR EXEC without MULTI");
                }
                std::vector<io::Reply> results;
                results.reserve(session.queue.size());
                for (const auto& queued : session.queue) {
                    results.push_back(Eval(eng, queued));
                }
                session.Reset();
                return io::MakeArray(std::move(results));
            },
            [&](const commands::Discard&) -> io::Reply {
                if (!session.in_multi) {
                    return io::MakeErr("ERR DISCARD without MULTI");
                }
                session.Reset();
                return io::Ok();
            },
            [&](const auto&) -> io::Reply {
                if (session.in_multi) {
                    session.queue.push_back(cmd);
                    return io::SimpleStr{"QUEUED"};
                }
                return Eval(eng, cmd);
            },
        },
        cmd);
}

}  // namespace valkey::engine
