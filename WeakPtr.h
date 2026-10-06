#ifndef HOMEWORK1_WEAKPTR_H
#define HOMEWORK1_WEAKPTR_H

#include <cstddef>
#include <utility>
#include "ShrdPtr.h"
#include "ControlBlock.h"

template<typename T>
class WeakPtr {
private:
    ControlBlock* block_;
public:
    WeakPtr() ;

    WeakPtr(const ShrdPtr<T>& s);
    WeakPtr(const WeakPtr& other);
    WeakPtr(WeakPtr&& other) noexcept;

    WeakPtr& operator=(const WeakPtr& other);
    WeakPtr& operator=(WeakPtr&& other) noexcept;
    WeakPtr& operator=(const ShrdPtr<T>& s);

    ~WeakPtr();

    bool expired() const;
    ShrdPtr<T> lock() const;
    size_t use_count() const;

    void reset();
    void swap(WeakPtr& other) ;
};

template<typename T>
WeakPtr<T>::WeakPtr()  : block_(nullptr) {}

template<typename T>
WeakPtr<T>::WeakPtr(const ShrdPtr<T>& s) : block_(s.block_) {
    if (block_) block_->incWeak();
}

template<typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& other) : block_(other.block_) {
    if (block_) block_->incWeak();
}

template<typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept : block_(other.block_) {
    other.block_ = nullptr;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
    if (this != &other) {
        WeakPtr tmp(other);
        swap(tmp);
    }
    return *this;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept {
    if (this != &other) {
        if (block_) block_->decWeak();
        block_ = other.block_;
        other.block_ = nullptr;
    }
    return *this;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const ShrdPtr<T>& s) {
    if (block_ == s.block_) return *this;
    if (block_) block_->decWeak();
    block_ = s.block_;
    if (block_) block_->incWeak();
    return *this;
}

template<typename T>
WeakPtr<T>::~WeakPtr() {
    if (block_) block_->decWeak();
}

template<typename T>
bool WeakPtr<T>::expired() const {
    return block_ == nullptr || block_->expired();
}

template<typename T>
ShrdPtr<T> WeakPtr<T>::lock() const {
    if (expired()) {
        return ShrdPtr<T>();
    }
    block_->incStrong();
    return ShrdPtr<T>(block_);
}

template<typename T>
size_t WeakPtr<T>::use_count() const {
    return block_ ? block_->useCount() : 0;
}

template<typename T>
void WeakPtr<T>::reset() {
    if (block_) {
        block_->decWeak();
        block_ = nullptr;
    }
}

template<typename T>
void WeakPtr<T>::swap(WeakPtr& other)  {
    std::swap(block_, other.block_);
}

#endif // HOMEWORK1_WEAKPTR_H