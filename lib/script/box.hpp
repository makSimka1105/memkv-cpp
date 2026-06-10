#pragma once

#include <memory>
#include <utility>

namespace valkey::script {

template <typename T>
class Box {
public:
    Box(const T& value) : ptr_(std::make_unique<T>(value)) {}
    Box(T&& value) : ptr_(std::make_unique<T>(std::move(value))) {}

    Box(const Box& other) : ptr_(std::make_unique<T>(*other.ptr_)) {}
    Box& operator=(const Box& other) {
        ptr_ = std::make_unique<T>(*other.ptr_);
        return *this;
    }

    Box(Box&&) noexcept = default;
    Box& operator=(Box&&) noexcept = default;

    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }

    T* operator->() { return ptr_.get(); }
    const T* operator->() const { return ptr_.get(); }

    T& Get() { return *ptr_; }
    const T& Get() const { return *ptr_; }

private:
    std::unique_ptr<T> ptr_;
};

template<typename T>
bool operator==(const Box<T>& l, const Box<T>& r) {
    return *l == *r;
}

}  // namespace valkey::script
