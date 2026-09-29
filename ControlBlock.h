#ifndef HOMEWORK1_CONTROLBLOCK_H
#define HOMEWORK1_CONTROLBLOCK_H

#include <cstddef>

class ControlBlock {
private:
    void* ptr_;
    size_t strong_;
    size_t weak_;
    void (*deleter_)(void*);

    ControlBlock(void* ptr, void (*deleter)(void*)) noexcept
        : ptr_(ptr), strong_(1), weak_(1), deleter_(deleter) {}

public:
    template<typename T>
    static ControlBlock* makeSingle(T* ptr) {
        return new ControlBlock(
            ptr,
            [](void* p) { delete static_cast<T*>(p); });
    }

    template<typename T>
    static ControlBlock* makeArray(T* ptr) {
        return new ControlBlock(
            ptr,
            [](void* p) { delete[] static_cast<T*>(p); });
    }

    void incStrong() noexcept {
        ++strong_;
        ++weak_;
    }

    void decStrong() noexcept {
        if (--strong_ == 0) {
            if (ptr_) {
                deleter_(ptr_);
                ptr_ = nullptr;
            }
        }
        if (--weak_ == 0) {
            delete this;
        }
    }

    void incWeak() noexcept { ++weak_; }

    void decWeak() noexcept {
        if (--weak_ == 0 && strong_ == 0) {
            delete this;
        }
    }

    void* get() const noexcept { return ptr_; }
    bool expired() const noexcept { return strong_ == 0; }
    size_t useCount() const noexcept { return strong_; }
};

#endif // HOMEWORK1_CONTROLBLOCK_H