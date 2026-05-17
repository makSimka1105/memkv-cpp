#pragma once

#include <concepts>
#include <cstddef>
#include <deque>
#include <string>
#include <unordered_set>

#include "meta/type_list.hpp"
#include "stored_types/all.hpp"

namespace valkey::storage {

using String = std::string;
using List = std::deque<std::string>;
using Set = std::unordered_set<std::string>;

using ValueVar = meta::VariantFromList<stored_types::AllTypes>;

template <typename T>
concept Stringy = requires(const T& v) {
    { v.size() } -> std::convertible_to<size_t>;
    { v.data() } -> std::convertible_to<const char*>;
    { v.empty() } -> std::convertible_to<bool>;
};

template <typename T>
concept Listy = requires(T& v, const T& cv, std::string s) {
    { cv.size() } -> std::convertible_to<size_t>;
    { cv.empty() } -> std::convertible_to<bool>;
    { v.push_front(s) };
    { v.push_back(s) };
    { v.pop_front() };
    { v.pop_back() };
};

template <typename T>
concept Setty = requires(T& v, const T& cv, std::string s) {
    { cv.size() } -> std::convertible_to<size_t>;
    { cv.empty() } -> std::convertible_to<bool>;
    { v.insert(s) };
    { v.erase(s) };
    { cv.contains(s) } -> std::convertible_to<bool>;
};

}  // namespace valkey::storage
