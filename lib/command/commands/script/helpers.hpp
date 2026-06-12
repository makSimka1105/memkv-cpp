#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "io/reply.hpp"

namespace valkey::commands {

struct KeysArgv {
    std::vector<std::string> keys;
    std::vector<std::string> argv;
};

std::expected<KeysArgv, io::Reply> SplitKeysArgv(const std::vector<std::string>& rest,
                                                  int64_t numkeys) {
    if (numkeys < 0) {
        return std::unexpected(io::MakeErr("ERR Number of keys can't be negative"));
    }
    if (static_cast<size_t>(numkeys) > rest.size()) {
        return std::unexpected(
            io::MakeErr("ERR Number of keys can't be greater than number of args"));
    }
    KeysArgv split;
    split.keys.assign(rest.begin(), rest.begin() + numkeys);
    split.argv.assign(rest.begin() + numkeys, rest.end());
    return split;
}

}  // namespace valkey::commands
