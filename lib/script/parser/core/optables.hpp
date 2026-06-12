#pragma once

#include <array>
#include <optional>
#include <utility>
#include <variant>

#include "script/ast.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct OpDesc {
    TkOp::OpKind token_op;
    BinOp::Op ast_op;
    int left_bp;
    int right_bp;
};

struct PrefixDesc {
    UnOp::Kind kind;
    int bp;
};

constexpr OpDesc kBinOps[] = {
    {TkOp::kEqEq, BinOp::kEq, 5, 6},       {TkOp::kNotEq, BinOp::kNeq, 5, 6},
    {TkOp::kLess, BinOp::kLt, 7, 8},       {TkOp::kLessEq, BinOp::kLe, 7, 8},
    {TkOp::kGreater, BinOp::kGt, 7, 8},    {TkOp::kGreaterEq, BinOp::kGe, 7, 8},
    {TkOp::kPlus, BinOp::kAdd, 9, 10},     {TkOp::kMinus, BinOp::kSub, 9, 10},
    {TkOp::kStar, BinOp::kMul, 11, 12},    {TkOp::kSlash, BinOp::kDiv, 11, 12},
    {TkOp::kPercent, BinOp::kMod, 11, 12},
};

constexpr std::pair<TkKw::Keyword, OpDesc> kKwBinOps[] = {
    {TkKw::kOr, {TkOp::kAssign, BinOp::kOr, 1, 2}},
    {TkKw::kAnd, {TkOp::kAssign, BinOp::kAnd, 3, 4}},
};

constexpr bool CoversAllBinOps() {
    std::array<int, BinOp::kLast> count{};
    for (const auto& desc : kBinOps) {
        ++count[desc.ast_op];
    }
    for (const auto& [kw, desc] : kKwBinOps) {
        ++count[desc.ast_op];
    }
    for (int c : count) {
        if (c != 1) {
            return false;
        }
    }
    return true;
}

static_assert(CoversAllBinOps(), "parser op tables do not cover BinOp");

std::optional<PrefixDesc> GetPrefixUnary(const Token& tok) {
    if (const auto* op = std::get_if<TkOp>(&tok)) {
        if (op->op == TkOp::kMinus) {
            return PrefixDesc{UnOp::kNeg, 13};
        }
    }
    if (const auto* kw = std::get_if<TkKw>(&tok)) {
        if (kw->kw == TkKw::kNot) {
            return PrefixDesc{UnOp::kNot, 13};
        }
    }
    return std::nullopt;
}

std::optional<OpDesc> GetOpInfo(const Token& tok) {
    if (const auto* op = std::get_if<TkOp>(&tok)) {
        for (const auto& d : kBinOps) {
            if (d.token_op == op->op) {
                return d;
            }
        }
    }
    if (const auto* kw = std::get_if<TkKw>(&tok)) {
        for (const auto& [k, d] : kKwBinOps) {
            if (k == kw->kw) {
                return d;
            }
        }
    }
    return std::nullopt;
}

}  // namespace valkey::script
