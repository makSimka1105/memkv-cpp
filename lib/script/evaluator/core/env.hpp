#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "script/value.hpp"

namespace valkey::script {

class Env {
public:
    void EnterScope() { scopes_.emplace_back(); }
    void LeaveScope() { scopes_.pop_back(); }

    void Declare(std::string name, ScriptValue value) {
        scopes_.back().insert_or_assign(std::move(name), std::move(value));
    }

    const ScriptValue* Lookup(std::string_view name) const {
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            if (auto it = scope->find(std::string(name)); it != scope->end()) {
                return &it->second;
            }
        }
        return nullptr;
    }

    bool Assign(std::string_view name, ScriptValue value) {
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            if (auto it = scope->find(std::string(name)); it != scope->end()) {
                it->second = std::move(value);
                return true;
            }
        }
        return false;
    }

private:
    std::vector<std::unordered_map<std::string, ScriptValue>> scopes_{1};
};

}  // namespace valkey::script
