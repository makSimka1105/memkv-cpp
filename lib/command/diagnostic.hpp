#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>

#include "util/str.hpp"

namespace valkey::parser {

struct DiagnosticMessage {
    std::string message;
};

template <typename T>
using CmdResult = std::expected<T, DiagnosticMessage>;

namespace err {

auto Message(std::string msg) {
    return std::unexpected(DiagnosticMessage{std::move(msg)});
}

auto EmptyInput() {
    return Message("ERR empty input");
}

auto SyntaxError() {
    return Message("ERR syntax error");
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

auto NotInteger() {
    return Message("ERR value is not an integer or out of range");
}

auto NotFloat() {
    return Message("ERR value is not a valid float");
}

auto UnexpectedEof() {
    return Message("unexpected EOF");
}

}  // namespace err
}  // namespace valkey::parser
