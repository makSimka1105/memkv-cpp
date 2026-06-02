#pragma once

#include <expected>
#include <utility>

#define VK_CONCAT_(a, b) a##b
#define VK_CONCAT(a, b) VK_CONCAT_(a, b)

#define VK_ASSIGN_OR_RETURN(lhs, rexpr)                                  \
    VK_ASSIGN_OR_RETURN_IMPL(VK_CONCAT(vk_tmp_, __COUNTER__), lhs, rexpr)
#define VK_ASSIGN_OR_RETURN_IMPL(tmp, lhs, rexpr)                        \
    auto tmp = (rexpr);                                                  \
    if (!tmp) return std::unexpected(std::move(tmp).error());            \
    lhs = std::move(*tmp)

#define VK_RETURN_IF_ERROR(rexpr)                                        \
    if (auto vk_st = (rexpr); !vk_st)                                    \
        return std::unexpected(std::move(vk_st).error())
