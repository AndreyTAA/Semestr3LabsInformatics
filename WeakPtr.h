#ifndef HOMEWORK1_WEAKPTR_H
#define HOMEWORK1_WEAKPTR_H


#include "ShrdPtr.h"
#include "ControlBlock.h"

template <typename T>
class WeakPtr {
    ControlBlock* block_;
public:
    WeakPtr(): block_(nullptr){}

    WeakPtr(const ShrdPtr<T>& s): block_(s.block_) {
        if (block_) block_->incWeak();
    }

    WeakPtr(const WeakPtr<T>& other): block_(other.block_) {
        if (block_) block_->incWeak();
    }

    WeakPtr(WeakPtr<T>&& other) noexcept : block_(other.block_) {
        other.block_ = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        WeakPtr tmp(other);
        swap(tmp);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (block_) block_->decWeak();   // ← отпускаем старый блок
            block_ = other.block_;           // ← забираем новый
            other.block_ = nullptr;
        }
        return *this;
    }

    WeakPtr& operator=(const ShrdPtr<T>& s) {
        if (block_ == s.block_) return *this;
        if (block_) block_->decWeak();
        block_ = s.block_;
        if (block_) block_->incWeak();
        return *this;
    }

    ~WeakPtr() {
        if (block_ != nullptr) {
            block_->decWeak();
        }
    }

    bool expired() const {
        return block_==nullptr || block_->expired();
    }

    ShrdPtr<T> lock() const {
        if (expired()) {
            return ShrdPtr<T>();
        }
        else {
            block_->incStrong();
            return ShrdPtr<T>(block_);
        }
    }

    size_t use_count() const {
        if (block_) {
            return block_->useCount();
        }
        return 0;
    }

    void reset() {
        if (block_) {
            block_->decWeak();
            block_= nullptr;
        }
    }

    void swap(WeakPtr& other) {
        std::swap(block_, other.block_);
    }
};

#endif // HOMEWORK1_WEAKPTR_H