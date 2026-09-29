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
    MsPtr() noexcept : buffer_(), size_(0), index_(0) {}

    MsPtr(const ShrdPtr<T[]>& buffer, size_t size, size_t index) noexcept
        : buffer_(buffer), size_(size), index_(index) {}

    T& operator*() const {
        if (!buffer_.get() || index_ >= size_) {
            throw std::out_of_range("MsPtr: dereference out of bounds");
        }
        return buffer_[index_];
    }



    T& operator[](ptrdiff_t offset) const {
        if (!buffer_.get()) {
            throw std::runtime_error("MsPtr: null buffer");
        }
        size_t newIndex = static_cast<size_t>(static_cast<ptrdiff_t>(index_) + offset);
        if (newIndex >= size_) {
            throw std::out_of_range("MsPtr: index out of bounds");
        }
        return buffer_[newIndex];
    }

    MsPtr& operator++() { ++index_; return *this; }
    MsPtr operator++(int) { MsPtr tmp = *this; ++index_; return tmp; }
    MsPtr& operator--() { --index_; return *this; }
    MsPtr operator--(int) { MsPtr tmp = *this; --index_; return tmp; }

    MsPtr& operator+=(ptrdiff_t offset) {
        index_ = static_cast<size_t>(static_cast<ptrdiff_t>(index_) + offset);
        return *this;
    }

    MsPtr& operator-=(ptrdiff_t offset) {
        index_ = static_cast<size_t>(static_cast<ptrdiff_t>(index_) - offset);
        return *this;
    }

    MsPtr operator+(ptrdiff_t offset) const {
        MsPtr tmp = *this;
        tmp += offset;
        return tmp;
    }

    MsPtr operator-(ptrdiff_t offset) const {
        MsPtr tmp = *this;
        tmp -= offset;
        return tmp;
    }

    ptrdiff_t operator-(const MsPtr& other) const {
        return static_cast<ptrdiff_t>(index_) - static_cast<ptrdiff_t>(other.index_);
    }

    bool operator==(const MsPtr& other) const noexcept {
        return buffer_.get() == other.buffer_.get() && index_ == other.index_;
    }

    bool operator!=(const MsPtr& other) const noexcept { return !(*this == other); }
    bool operator<(const MsPtr& other) const noexcept { return index_ < other.index_; }
    bool operator>(const MsPtr& other) const noexcept { return index_ > other.index_; }
    bool operator<=(const MsPtr& other) const noexcept { return index_ <= other.index_; }
    bool operator>=(const MsPtr& other) const noexcept { return index_ >= other.index_; }

    explicit operator bool() const noexcept {
        return buffer_.get() != nullptr;
    }

    size_t index() const noexcept { return index_; }
    size_t size() const noexcept { return size_; }
};

#endif // HOMEWORK1_MSPTR_H