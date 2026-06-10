#pragma once

#include <expected>
#include <string>
#include <utility>

namespace valkey::script {

enum class ErrKind {
    kLex,
    kParse,
    kType,
    kDivZero,
    kArity,
    kIndex,
    kIterLimit,
    kName,
    kCall,
};

struct ScriptError {
    ErrKind kind;
    std::string message;
};

std::unexpected<ScriptError> MakeError(ErrKind kind, std::string message) {
    return std::unexpected<ScriptError>(ScriptError{kind, std::move(message)});
}

}  // namespace valkey::script
