#ifndef HOMEWORK1_MEMORYSPAN_H
#define HOMEWORK1_MEMORYSPAN_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include "ShrdPtr.h"
#include "UnqPtr.h"
#include "MsPtr.h"

template<typename T>
class MemorySpan {
private:
    ShrdPtr<T[]> buffer_;
    size_t size_;
    size_t capacity_;

    void grow();

public:
    explicit MemorySpan(size_t initialCapacity = 4);

    template<typename U>
    void append(U&& value);

    template<typename U>
    void push_back(U&& value);

    size_t size() const ;
    size_t capacity() const ;
    bool empty() const ;

    T* raw() ;
    const T* raw() const ;

    UnqPtr<T> Get(size_t index) const;
    ShrdPtr<T> Copy(size_t index) const;
    MsPtr<T> Locate(size_t index) ;

    MsPtr<T> begin() ;
    MsPtr<T> end() ;
};

template<typename T>
void MemorySpan<T>::grow() {
    size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
    ShrdPtr<T[]> newBuffer = makeShrdArray<T>(newCapacity);
    for (size_t i = 0; i < size_; ++i) {
        newBuffer[i] = std::move(buffer_[i]);
    }
    buffer_ = newBuffer;
    capacity_ = newCapacity;
}

template<typename T>
MemorySpan<T>::MemorySpan(size_t initialCapacity)
    : buffer_(makeShrdArray<T>(initialCapacity)),
      size_(0),
      capacity_(initialCapacity) {}

template<typename T>
template<typename U>
void MemorySpan<T>::append(U&& value) {
    if (size_ == capacity_) grow();
    buffer_[size_++] = std::forward<U>(value);
}

template<typename T>
template<typename U>
void MemorySpan<T>::push_back(U&& value) {
    append(std::forward<U>(value));
}

template<typename T>
size_t MemorySpan<T>::size() const  {
    return size_;
}

template<typename T>
size_t MemorySpan<T>::capacity() const  {
    return capacity_;
}

template<typename T>
bool MemorySpan<T>::empty() const  {
    return size_ == 0;
}

template<typename T>
T* MemorySpan<T>::raw()  {
    return buffer_.get();
}

template<typename T>
const T* MemorySpan<T>::raw() const  {
    return buffer_.get();
}

template<typename T>
UnqPtr<T> MemorySpan<T>::Get(size_t index) const {
    if (index >= size_) throw std::out_of_range("MemorySpan::Get");
    return makeUnq<T>(buffer_[index]);
}

template<typename T>
ShrdPtr<T> MemorySpan<T>::Copy(size_t index) const {
    if (index >= size_) throw std::out_of_range("MemorySpan::Copy");
    return makeShrd<T>(buffer_[index]);
}

template<typename T>
MsPtr<T> MemorySpan<T>::Locate(size_t index)  {
    return MsPtr<T>(buffer_, size_, index);
}

template<typename T>
MsPtr<T> MemorySpan<T>::begin()  {
    return MsPtr<T>(buffer_, size_, 0);
}

template<typename T>
MsPtr<T> MemorySpan<T>::end()  {
    return MsPtr<T>(buffer_, size_, size_);
}

#endif // HOMEWORK1_MEMORYSPAN_H