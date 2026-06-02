#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <utility>

#include "util/str.hpp"

namespace valkey::parser {

struct ParseError {
    std::string message;
    size_t token_index = 0;
};

namespace err {

auto Message(std::string msg, size_t pos = 0) {
    return std::unexpected(ParseError{std::move(msg), pos});
}

auto EmptyInput() {
    return Message("ERR empty input");
}

auto SyntaxError(size_t pos = 0) {
    return Message("ERR syntax error", pos);
}

auto WrongArgs(std::string_view name) {
    return Message("ERR wrong number of arguments for '" + LowerName(name) + "' command");
}

auto UnknownCommand(std::string_view word) {
    return Message("ERR unknown command '" + std::string(word) + "'");
}

auto UnknownSubcommand(std::string_view word) {
    return Message("ERR unknown subcommand '" + std::string(word) + "'");
}

auto NotInteger(size_t pos) {
    return Message("ERR value is not an integer or out of range", pos);
}

auto NotFloat(size_t pos) {
    return Message("ERR value is not a valid float", pos);
}

auto UnexpectedEof(size_t pos) {
    return Message("unexpected EOF", pos);
}

}  // namespace err
}  // namespace valkey::parser
