#ifndef HOMEWORK1_SHRDPTR_H
#define HOMEWORK1_SHRDPTR_H

#include <cstddef>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include "ControlBlock.h"

template<typename T> class WeakPtr;
template<typename T> class ShrdPtr;
template<typename T> ShrdPtr<T[]> makeShrdArray(size_t count);

template<typename T>
class ShrdPtr {
private:
    ControlBlock* block_;

    template<typename U> friend class ShrdPtr;
    template<typename U> friend class WeakPtr;

    explicit ShrdPtr(ControlBlock* block) ;
    explicit ShrdPtr(T* ptr);

    template<typename U, typename... Args>
    friend ShrdPtr<U> makeShrd(Args&&... args);

public:
    ShrdPtr() ;
    ShrdPtr(std::nullptr_t) ;

    ShrdPtr(const ShrdPtr& other) ;

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    ShrdPtr(const ShrdPtr<U>& other) ;

    ShrdPtr(ShrdPtr&& other) noexcept;

    ShrdPtr& operator=(const ShrdPtr& other) ;
    ShrdPtr& operator=(ShrdPtr&& other) noexcept;
    ShrdPtr& operator=(std::nullptr_t) ;

    ~ShrdPtr();

    T* get() const ;
    T& operator*() const;
    T* operator->() const ;
    explicit operator bool() const ;
    size_t use_count() const ;

    void reset(T* p = nullptr);
    void swap(ShrdPtr& other) ;
};

template<typename T>
ShrdPtr<T>::ShrdPtr(ControlBlock* block)  : block_(block) {}

template<typename T>
ShrdPtr<T>::ShrdPtr(T* ptr)
    : block_(ptr ? ControlBlock::makeSingle(ptr) : nullptr) {}

template<typename T>
ShrdPtr<T>::ShrdPtr()  : block_(nullptr) {}

template<typename T>
ShrdPtr<T>::ShrdPtr(std::nullptr_t)  : block_(nullptr) {}

template<typename T>
ShrdPtr<T>::ShrdPtr(const ShrdPtr& other)  : block_(other.block_) {
    if (block_) block_->incStrong();
}

template<typename T>
template<typename U, typename>
ShrdPtr<T>::ShrdPtr(const ShrdPtr<U>& other)  : block_(other.block_) {
    if (block_) block_->incStrong();
}

template<typename T>
ShrdPtr<T>::ShrdPtr(ShrdPtr&& other) noexcept : block_(other.block_) {
    other.block_ = nullptr;
}

template<typename T>
ShrdPtr<T>& ShrdPtr<T>::operator=(const ShrdPtr& other)  {
    if (this != &other) {
        ShrdPtr tmp(other);
        swap(tmp);
    }
    return *this;
}

template<typename T>
ShrdPtr<T>& ShrdPtr<T>::operator=(ShrdPtr&& other) noexcept {
    if (this != &other) {
        ShrdPtr tmp(std::move(other));
        swap(tmp);
    }
    return *this;
}

template<typename T>
ShrdPtr<T>& ShrdPtr<T>::operator=(std::nullptr_t)  {
    reset();
    return *this;
}

template<typename T>
ShrdPtr<T>::~ShrdPtr() {
    if (block_) block_->decStrong();
}

template<typename T>
T* ShrdPtr<T>::get() const  {
    return block_ ? static_cast<T*>(block_->get()) : nullptr;
}

template<typename T>
T& ShrdPtr<T>::operator*() const {
    T* p = get();
    if (p == nullptr) throw std::runtime_error("dereference null ShrdPtr");
    return *p;
}

template<typename T>
T* ShrdPtr<T>::operator->() const  {
    return get();
}

template<typename T>
ShrdPtr<T>::operator bool() const  {
    return get() != nullptr;
}

template<typename T>
size_t ShrdPtr<T>::use_count() const  {
    return block_ ? block_->useCount() : 0;
}

template<typename T>
void ShrdPtr<T>::reset(T* p) {
    ShrdPtr tmp(p);
    swap(tmp);
}

template<typename T>
void ShrdPtr<T>::swap(ShrdPtr& other)  {
    std::swap(block_, other.block_);
}

template<typename T, typename... Args>
ShrdPtr<T> makeShrd(Args&&... args) {
    return ShrdPtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
class ShrdPtr<T[]> {
private:
    ControlBlock* block_;

    explicit ShrdPtr(ControlBlock* block) ;
    explicit ShrdPtr(T* ptr);

    template<typename U>
    friend ShrdPtr<U[]> makeShrdArray(size_t count);

public:
    ShrdPtr() ;
    ShrdPtr(std::nullptr_t) ;

    ShrdPtr(const ShrdPtr& other) ;
    ShrdPtr(ShrdPtr&& other) noexcept;

    ShrdPtr& operator=(const ShrdPtr& other) ;
    ShrdPtr& operator=(ShrdPtr&& other) noexcept;
    ShrdPtr& operator=(std::nullptr_t) ;

    ~ShrdPtr();

    T* get() const ;
    T& operator[](size_t index) const;
    explicit operator bool() const ;
    size_t use_count() const ;

    void reset();
    void swap(ShrdPtr& other) ;
};

template<typename T>
ShrdPtr<T[]>::ShrdPtr(ControlBlock* block)  : block_(block) {}

template<typename T>
ShrdPtr<T[]>::ShrdPtr(T* ptr)
    : block_(ptr ? ControlBlock::makeArray(ptr) : nullptr) {}

template<typename T>
ShrdPtr<T[]>::ShrdPtr()  : block_(nullptr) {}

template<typename T>
ShrdPtr<T[]>::ShrdPtr(std::nullptr_t)  : block_(nullptr) {}

template<typename T>
ShrdPtr<T[]>::ShrdPtr(const ShrdPtr& other)  : block_(other.block_) {
    if (block_) block_->incStrong();
}

template<typename T>
ShrdPtr<T[]>::ShrdPtr(ShrdPtr&& other) noexcept : block_(other.block_) {
    other.block_ = nullptr;
}

template<typename T>
ShrdPtr<T[]>& ShrdPtr<T[]>::operator=(const ShrdPtr& other)  {
    if (this != &other) {
        ShrdPtr tmp(other);
        swap(tmp);
    }
    return *this;
}

template<typename T>
ShrdPtr<T[]>& ShrdPtr<T[]>::operator=(ShrdPtr&& other) noexcept {
    if (this != &other) {
        ShrdPtr tmp(std::move(other));
        swap(tmp);
    }
    return *this;
}

template<typename T>
ShrdPtr<T[]>& ShrdPtr<T[]>::operator=(std::nullptr_t)  {
    reset();
    return *this;
}

template<typename T>
ShrdPtr<T[]>::~ShrdPtr() {
    if (block_) block_->decStrong();
}

template<typename T>
T* ShrdPtr<T[]>::get() const  {
    return block_ ? static_cast<T*>(block_->get()) : nullptr;
}

template<typename T>
T& ShrdPtr<T[]>::operator[](size_t index) const {
    return get()[index];
}

template<typename T>
ShrdPtr<T[]>::operator bool() const  {
    return get() != nullptr;
}

template<typename T>
size_t ShrdPtr<T[]>::use_count() const  {
    return block_ ? block_->useCount() : 0;
}

template<typename T>
void ShrdPtr<T[]>::reset() {
    ShrdPtr tmp;
    swap(tmp);
}

template<typename T>
void ShrdPtr<T[]>::swap(ShrdPtr& other)  {
    std::swap(block_, other.block_);
}

template<typename T>
ShrdPtr<T[]> makeShrdArray(size_t count) {
    return ShrdPtr<T[]>(new T[count]());
}

#endif // HOMEWORK1_SHRDPTR_H