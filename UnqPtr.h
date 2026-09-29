#ifndef HOMEWORK1_UNQPTR_H
#define HOMEWORK1_UNQPTR_H

#include <cstddef>
#include <utility>
#include <stdexcept>
#include <type_traits>

template<typename T>
class UnqPtr {
private:
    T* ptr_;

    template<typename U>
    friend class UnqPtr;

    explicit UnqPtr(T* ptr) noexcept : ptr_(ptr) {}

    template<typename U, typename... Args>
    friend UnqPtr<U> makeUnq(Args&&... args);
public:
    UnqPtr() noexcept : ptr_(nullptr) {}

    UnqPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}

    UnqPtr(const UnqPtr& other) = delete;

    UnqPtr& operator=(const UnqPtr& other) = delete;

    UnqPtr(UnqPtr&& other) noexcept : ptr_(other.release()) {}

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    UnqPtr(UnqPtr<U>&& other) noexcept : ptr_(other.release()) {}

    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    UnqPtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    ~UnqPtr() {
        delete ptr_;
    }

    T* get()  noexcept {
        return ptr_;
    }

    T& operator*() const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("dereference null UnqPtr");
        }
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) noexcept {
        if (p != ptr_) {
            delete ptr_;
            ptr_ = p;
        }
    }

    void swap(UnqPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }
};

template<typename T>
class UnqPtr<T[]> {
private:
    T* ptr_;

    template<typename U>
    friend class UnqPtr;

    explicit UnqPtr(T* ptr) noexcept : ptr_(ptr) {}
public:
    UnqPtr() noexcept : ptr_(nullptr) {}

    UnqPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}

    UnqPtr(const UnqPtr& other) = delete;

    UnqPtr& operator=(const UnqPtr& other) = delete;

    UnqPtr(UnqPtr&& other) noexcept : ptr_(other.release()) {}

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    UnqPtr(UnqPtr<U>&& other) noexcept : ptr_(other.release()) {}

    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    UnqPtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    ~UnqPtr() {
        delete[] ptr_;
    }

    T* get()  noexcept {
        return ptr_;
    }

    T& operator[](size_t index) const {
        return ptr_[index];
    }

    operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) noexcept {
        if (p != ptr_) {
            delete[] ptr_;
            ptr_ = p;
        }
    }

    void swap(UnqPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }
};

template<typename T, typename... Args>
UnqPtr<T> makeUnq(Args&&... args) {
    return UnqPtr<T>(new T(std::forward<Args>(args)...));
}


#endif // HOMEWORK1_UNQPTR_H