#pragma once

#include <string>
#include <string_view>

#include "io/reply.hpp"

namespace valkey::engine {

constexpr std::string_view kWrongType =
    "WRONGTYPE Operation against a key holding the wrong kind of value";
constexpr std::string_view kOom =
    "OOM command not allowed when used memory > 'maxmemory'";

io::Reply WrongTypeReply() { return io::MakeErr(std::string{kWrongType}); }
io::Reply OomReply() { return io::MakeErr(std::string{kOom}); }

}  // namespace valkey::engine
