#ifndef HOMEWORK1_MSPTR_H
#define HOMEWORK1_MSPTR_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include "ShrdPtr.h"

template<typename T>
class MsPtr {
private:
    ShrdPtr<T[]> buffer_;
    size_t size_;
    size_t index_;
public:
    MsPtr() ;
    MsPtr(const ShrdPtr<T[]>& buffer, size_t size, size_t index) ;

    T& operator*() const;
    T& operator[](ptrdiff_t offset) const;

    MsPtr& operator++();
    MsPtr operator++(int);
    MsPtr& operator--();
    MsPtr operator--(int);

    MsPtr& operator+=(ptrdiff_t offset);
    MsPtr& operator-=(ptrdiff_t offset);

    MsPtr operator+(ptrdiff_t offset) const;
    MsPtr operator-(ptrdiff_t offset) const;
    ptrdiff_t operator-(const MsPtr& other) const;

    bool operator==(const MsPtr& other) const ;
    bool operator!=(const MsPtr& other) const ;
    bool operator<(const MsPtr& other) const ;
    bool operator>(const MsPtr& other) const ;
    bool operator<=(const MsPtr& other) const ;
    bool operator>=(const MsPtr& other) const ;

    explicit operator bool() const ;

    size_t index() const ;
    size_t size() const ;
};

template<typename T>
MsPtr<T>::MsPtr()  : buffer_(), size_(0), index_(0) {}

template<typename T>
MsPtr<T>::MsPtr(const ShrdPtr<T[]>& buffer, size_t size, size_t index)
    : buffer_(buffer), size_(size), index_(index) {}

template<typename T>
T& MsPtr<T>::operator*() const {
    if (!buffer_.get() || index_ >= size_) {
        throw std::out_of_range("MsPtr: dereference out of bounds");
    }
    return buffer_[index_];
}

template<typename T>
T& MsPtr<T>::operator[](ptrdiff_t offset) const {
    if (!buffer_.get()) {
        throw std::runtime_error("MsPtr: null buffer");
    }
    size_t newIndex = static_cast<size_t>(static_cast<ptrdiff_t>(index_) + offset);
    if (newIndex >= size_) {
        throw std::out_of_range("MsPtr: index out of bounds");
    }
    return buffer_[newIndex];
}

template<typename T>
MsPtr<T>& MsPtr<T>::operator++() {
    ++index_;
    return *this;
}

template<typename T>
MsPtr<T> MsPtr<T>::operator++(int) {
    MsPtr tmp = *this;
    ++index_;
    return tmp;
}

template<typename T>
MsPtr<T>& MsPtr<T>::operator--() {
    --index_;
    return *this;
}

template<typename T>
MsPtr<T> MsPtr<T>::operator--(int) {
    MsPtr tmp = *this;
    --index_;
    return tmp;
}

template<typename T>
MsPtr<T>& MsPtr<T>::operator+=(ptrdiff_t offset) {
    index_ = static_cast<size_t>(static_cast<ptrdiff_t>(index_) + offset);
    return *this;
}

template<typename T>
MsPtr<T>& MsPtr<T>::operator-=(ptrdiff_t offset) {
    index_ = static_cast<size_t>(static_cast<ptrdiff_t>(index_) - offset);
    return *this;
}

template<typename T>
MsPtr<T> MsPtr<T>::operator+(ptrdiff_t offset) const {
    MsPtr tmp = *this;
    tmp += offset;
    return tmp;
}

template<typename T>
MsPtr<T> MsPtr<T>::operator-(ptrdiff_t offset) const {
    MsPtr tmp = *this;
    tmp -= offset;
    return tmp;
}

template<typename T>
ptrdiff_t MsPtr<T>::operator-(const MsPtr& other) const {
    return static_cast<ptrdiff_t>(index_) - static_cast<ptrdiff_t>(other.index_);
}

template<typename T>
bool MsPtr<T>::operator==(const MsPtr& other) const  {
    return buffer_.get() == other.buffer_.get() && index_ == other.index_;
}

template<typename T>
bool MsPtr<T>::operator!=(const MsPtr& other) const  {
    return !(*this == other);
}

template<typename T>
bool MsPtr<T>::operator<(const MsPtr& other) const  {
    return index_ < other.index_;
}

template<typename T>
bool MsPtr<T>::operator>(const MsPtr& other) const  {
    return index_ > other.index_;
}

template<typename T>
bool MsPtr<T>::operator<=(const MsPtr& other) const  {
    return index_ <= other.index_;
}

template<typename T>
bool MsPtr<T>::operator>=(const MsPtr& other) const  {
    return index_ >= other.index_;
}

template<typename T>
MsPtr<T>::operator bool() const  {
    return buffer_.get() != nullptr;
}

template<typename T>
size_t MsPtr<T>::index() const  {
    return index_;
}

template<typename T>
size_t MsPtr<T>::size() const  {
    return size_;
}

#endif // HOMEWORK1_MSPTR_H