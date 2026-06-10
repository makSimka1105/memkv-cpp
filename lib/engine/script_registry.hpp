#pragma once

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "io/reply.hpp"

namespace valkey::engine {

using ScriptFn =
    std::function<io::Reply(std::span<const std::string>, std::span<const std::string>)>;

class ScriptRegistry {
public:
    void Register(std::string name, ScriptFn fn) { functions_[std::move(name)] = std::move(fn); }

    const ScriptFn* Find(std::string_view name) const {
        const auto it = functions_.find(std::string(name));
        return it == functions_.end() ? nullptr : &it->second;
    }

    bool Delete(std::string_view name) { return functions_.erase(std::string(name)) != 0; }

private:
    std::unordered_map<std::string, ScriptFn> functions_;
};

}  // namespace valkey::engine
