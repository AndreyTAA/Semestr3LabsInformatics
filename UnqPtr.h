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

    template<typename U> friend class UnqPtr;

    explicit UnqPtr(T* ptr) ;

    template<typename U, typename... Args>
    friend UnqPtr<U> makeUnq(Args&&... args);

public:
    UnqPtr() ;
    UnqPtr(std::nullptr_t) ;

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept;

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    UnqPtr(UnqPtr<U>&& other) noexcept;

    UnqPtr& operator=(UnqPtr&& other) noexcept;
    UnqPtr& operator=(std::nullptr_t) ;

    ~UnqPtr();

    T* get() const ;
    T& operator*() const;
    T* operator->() const ;
    explicit operator bool() const ;

    T* release() ;
    void reset(T* p = nullptr) ;
    void swap(UnqPtr& other) ;
};

template<typename T>
UnqPtr<T>::UnqPtr(T* ptr)  : ptr_(ptr) {}

template<typename T>
UnqPtr<T>::UnqPtr()  : ptr_(nullptr) {}

template<typename T>
UnqPtr<T>::UnqPtr(std::nullptr_t)  : ptr_(nullptr) {}

template<typename T>
UnqPtr<T>::UnqPtr(UnqPtr&& other) noexcept : ptr_(other.release()) {}

template<typename T>
template<typename U, typename>
UnqPtr<T>::UnqPtr(UnqPtr<U>&& other) noexcept : ptr_(other.release()) {}

template<typename T>
UnqPtr<T>& UnqPtr<T>::operator=(UnqPtr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

template<typename T>
UnqPtr<T>& UnqPtr<T>::operator=(std::nullptr_t)  {
    reset();
    return *this;
}

template<typename T>
UnqPtr<T>::~UnqPtr() {
    delete ptr_;
}

template<typename T>
T* UnqPtr<T>::get() const  {
    return ptr_;
}

template<typename T>
T& UnqPtr<T>::operator*() const {
    if (ptr_ == nullptr) {
        throw std::runtime_error("dereference null UnqPtr");
    }
    return *ptr_;
}

template<typename T>
T* UnqPtr<T>::operator->() const  {
    return ptr_;
}

template<typename T>
UnqPtr<T>::operator bool() const  {
    return ptr_ != nullptr;
}

template<typename T>
T* UnqPtr<T>::release()  {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
}

template<typename T>
void UnqPtr<T>::reset(T* p)  {
    if (p != ptr_) {
        delete ptr_;
        ptr_ = p;
    }
}

template<typename T>
void UnqPtr<T>::swap(UnqPtr& other)  {
    std::swap(ptr_, other.ptr_);
}

template<typename T, typename... Args>
UnqPtr<T> makeUnq(Args&&... args) {
    return UnqPtr<T>(new T(std::forward<Args>(args)...));
}

#endif // HOMEWORK1_UNQPTR_H