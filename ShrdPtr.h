#ifndef HOMEWORK1_SHRDPTR_H
#define HOMEWORK1_SHRDPTR_H

#include <cstddef>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include "ControlBlock.h"

template<typename T>
class WeakPtr;

template<typename T>
class ShrdPtr;

template<typename T>
ShrdPtr<T[]> makeShrdArray(size_t count);

template<typename T>
class ShrdPtr {
private:
    ControlBlock* block_;

    template<typename U> friend class ShrdPtr;
    template<typename U> friend class WeakPtr;

    explicit ShrdPtr(ControlBlock* block) noexcept : block_(block) {}

    explicit ShrdPtr(T* ptr)
        : block_(ptr ? ControlBlock::makeSingle(ptr) : nullptr) {}

    template<typename U, typename... Args>
    friend ShrdPtr<U> makeShrd(Args&&... args);

public:
    ShrdPtr() noexcept : block_(nullptr) {}
    ShrdPtr(std::nullptr_t) noexcept : block_(nullptr) {}

    ShrdPtr(const ShrdPtr& other) noexcept : block_(other.block_) {
        if (block_) block_->incStrong();
    }

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    ShrdPtr(const ShrdPtr<U>& other) noexcept : block_(other.block_) {
        if (block_) block_->incStrong();
    }

    ShrdPtr(ShrdPtr&& other) noexcept : block_(other.block_) {
        other.block_ = nullptr;
    }

    ShrdPtr& operator=(const ShrdPtr& other) noexcept {
        if (this != &other) { ShrdPtr tmp(other); swap(tmp); }
        return *this;
    }

    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) { ShrdPtr tmp(std::move(other)); swap(tmp); }
        return *this;
    }

    ShrdPtr& operator=(std::nullptr_t) noexcept { reset(); return *this; }

    ~ShrdPtr() {
        if (block_) block_->decStrong();
    }

    T* get() const noexcept {
        return block_ ? static_cast<T*>(block_->get()) : nullptr;
    }

    T& operator*() const {
        T* p = get();
        if (p == nullptr) throw std::runtime_error("dereference null ShrdPtr");
        return *p;
    }

    T* operator->() const noexcept { return get(); }

    explicit operator bool() const noexcept { return get() != nullptr; }

    size_t use_count() const noexcept {
        return block_ ? block_->useCount() : 0;
    }

    void reset(T* p = nullptr) {
        ShrdPtr tmp(p);
        swap(tmp);
    }

    void swap(ShrdPtr& other) noexcept {
        std::swap(block_, other.block_);
    }
};

template<typename T, typename... Args>
ShrdPtr<T> makeShrd(Args&&... args) {
    return ShrdPtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
class ShrdPtr<T[]> {
private:
    ControlBlock* block_;

    explicit ShrdPtr(ControlBlock* block) noexcept : block_(block) {}

    explicit ShrdPtr(T* ptr)
        : block_(ptr ? ControlBlock::makeArray(ptr) : nullptr) {}

    template<typename U>
    friend ShrdPtr<U[]> makeShrdArray(size_t count);

public:
    ShrdPtr() noexcept : block_(nullptr) {}
    ShrdPtr(std::nullptr_t) noexcept : block_(nullptr) {}

    ShrdPtr(const ShrdPtr& other) noexcept : block_(other.block_) {
        if (block_) block_->incStrong();
    }

    ShrdPtr(ShrdPtr&& other) noexcept : block_(other.block_) {
        other.block_ = nullptr;
    }

    ShrdPtr& operator=(const ShrdPtr& other) noexcept {
        if (this != &other) { ShrdPtr tmp(other); swap(tmp); }
        return *this;
    }

    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) { ShrdPtr tmp(std::move(other)); swap(tmp); }
        return *this;
    }

    ShrdPtr& operator=(std::nullptr_t) noexcept { reset(); return *this; }

    ~ShrdPtr() {
        if (block_) block_->decStrong();
    }

    T* get() const noexcept {
        return block_ ? static_cast<T*>(block_->get()) : nullptr;
    }

    T& operator[](size_t index) const { return get()[index]; }

    explicit operator bool() const noexcept { return get() != nullptr; }

    size_t use_count() const noexcept {
        return block_ ? block_->useCount() : 0;
    }

    void reset() { ShrdPtr tmp; swap(tmp); }
    void swap(ShrdPtr& other) noexcept { std::swap(block_, other.block_); }
};

template<typename T>
ShrdPtr<T[]> makeShrdArray(size_t count) {
    return ShrdPtr<T[]>(new T[count]());
}

#endif // HOMEWORK1_SHRDPTR_H