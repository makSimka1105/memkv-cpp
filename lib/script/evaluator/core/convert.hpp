#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "engine/script_registry.hpp"
#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "meta/try.hpp"
#include "script/box.hpp"
#include "script/error.hpp"
#include "script/value.hpp"

namespace valkey::script {

io::Reply ToReply(const ScriptValue& value) {
    return std::visit(
        Overload{
            [](Nil) -> io::Reply { return io::Nil{}; },
            [](bool flag) -> io::Reply { return io::Bool{flag}; },
            [](int64_t number) -> io::Reply { return io::Int{number}; },
            [](double number) -> io::Reply { return io::BulkStr{std::to_string(number)}; },
            [](const std::string& text) -> io::Reply { return io::BulkStr{text}; },
            [](const Box<List>& list) -> io::Reply {
                std::vector<io::Reply> items;
                items.reserve(list->items.size());
                for (const auto& item : list->items) {
                    items.push_back(ToReply(item));
                }
                return io::MakeArray(std::move(items));
            },
        },
        value);
}

ValueResult FromReply(const io::Reply& reply) {
    return std::visit(
        Overload{
            [](const io::SimpleStr& s) -> ValueResult { return s.value; },
            [](const io::BulkStr& s) -> ValueResult { return s.value; },
            [](const io::Int& n) -> ValueResult { return n.value; },
            [](const io::Bool& b) -> ValueResult { return b.value; },
            [](io::Nil) -> ValueResult { return Nil{}; },
            [](const io::Err& e) -> ValueResult { return MakeError(ErrKind::kCall, e.message); },
            [](const std::unique_ptr<io::Array>& array) -> ValueResult {
                List list;
                list.items.reserve(array->items.size());
                for (const auto& item : array->items) {
                    VK_ASSIGN_OR_RETURN(auto value, FromReply(item));
                    list.items.push_back(std::move(value));
                }
                return ScriptValue{Box<List>{std::move(list)}};
            },
        },
        reply);
}

std::expected<std::string, ScriptError> ToArg(const ScriptValue& value) {
    using Result = std::expected<std::string, ScriptError>;
    return std::visit(
        Overload{
            [](const std::string& text) -> Result { return text; },
            [](int64_t number) -> Result { return std::to_string(number); },
            [](double number) -> Result { return std::to_string(number); },
            [](bool flag) -> Result { return std::string(flag ? "1" : "0"); },
            [](Nil) -> Result {
                return MakeError(ErrKind::kType, "cannot pass nil as command argument");
            },
            [](const Box<List>&) -> Result {
                return MakeError(ErrKind::kType, "cannot pass list as command argument");
            },
        },
        value);
}

std::expected<std::vector<std::string>, ScriptError> ArgsToStrings(
    std::span<const ScriptValue> args) {
    std::vector<std::string> result;
    result.reserve(args.size());
    for (const auto& arg : args) {
        VK_ASSIGN_OR_RETURN(auto text, ToArg(arg));
        result.push_back(std::move(text));
    }
    return result;
}

ValueResult CallScriptFn(const engine::ScriptFn& fn, std::span<const ScriptValue> args) {
    VK_ASSIGN_OR_RETURN(auto argv, ArgsToStrings(args));
    std::vector<std::string> keys;
    return FromReply(fn(keys, argv));
}

}  // namespace valkey::script
