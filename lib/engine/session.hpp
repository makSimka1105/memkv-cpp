#pragma once

#include <vector>

#include "command/parser/cmd.hpp"

namespace valkey::engine {

struct Session {
    bool in_multi = false;
    std::vector<parser::Cmd> queue;

    void Reset() {
        in_multi = false;
        queue.clear();
    }
};

}  // namespace valkey::engine
