#pragma once

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace valkey::io {

struct SimpleStr {
    std::string value;
};

struct BulkStr {
    std::string value;
};

struct Int {
    int64_t value;
};

struct Bool {
    bool value;
};

struct Err {
    std::string message;
};

struct Nil {};

struct Array;

using Reply = std::variant<SimpleStr, BulkStr, Int, Bool, Err, Nil, std::unique_ptr<Array>>;

struct Array {
    std::vector<Reply> items;
};

Reply Ok() {
    return SimpleStr{"OK"};
}

Reply MakeNil() {
    return Nil{};
}

Reply MakeInt(int64_t v) {
    return Int{v};
}

Reply MakeBool(bool v) {
    return Bool{v};
}

Reply MakeBulk(std::string v) {
    return BulkStr{std::move(v)};
}

Reply MakeBulk(double v) {
    return BulkStr{std::format("{:.2f}", v)};
}

Reply MakeErr(std::string m) {
    return Err{std::move(m)};
}

Reply MakeArray(std::vector<Reply> xs) {
    return std::make_unique<Array>(Array{std::move(xs)});
}

}  // namespace valkey::io
