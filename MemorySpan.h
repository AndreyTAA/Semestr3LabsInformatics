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

    void grow() {
        size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
        ShrdPtr<T[]> newBuffer = makeShrdArray<T>(newCapacity);
        for (size_t i = 0; i < size_; ++i) {
            newBuffer[i] = std::move(buffer_[i]);
        }
        buffer_ = newBuffer;
        capacity_ = newCapacity;
    }

public:
    explicit MemorySpan(size_t initialCapacity = 4)
        : buffer_(makeShrdArray<T>(initialCapacity)),
          size_(0),
          capacity_(initialCapacity) {}

    template<typename U>
    void append(U&& value) {
        if (size_ == capacity_) grow();
        buffer_[size_++] = std::forward<U>(value);
    }

    template<typename U>
    void push_back(U&& value) { append(std::forward<U>(value)); }

    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    T* raw() noexcept { return buffer_.get(); }
    const T* raw() const noexcept { return buffer_.get(); }

    UnqPtr<T> Get(size_t index) const {
        if (index >= size_) throw std::out_of_range("MemorySpan::Get");
        return makeUnq<T>(buffer_[index]);
    }

    ShrdPtr<T> Copy(size_t index) const {
        if (index >= size_) throw std::out_of_range("MemorySpan::Copy");
        return makeShrd<T>(buffer_[index]);
    }

    MsPtr<T> Locate(size_t index) noexcept {
        return MsPtr<T>(buffer_, size_, index);
    }

    MsPtr<T> begin() noexcept { return MsPtr<T>(buffer_, size_, 0); }
    MsPtr<T> end() noexcept { return MsPtr<T>(buffer_, size_, size_); }
};

#endif // HOMEWORK1_MEMORYSPAN_H