#ifndef HOMEWORK1_DYNAMICARRAY_H
#define HOMEWORK1_DYNAMICARRAY_H

#include <cstddef>
#include <stdexcept>
#include "MemorySpan.h"
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "MsPtr.h"

template<typename T>
class DynamicArray {
private:
    MemorySpan<T> span_;

public:
    DynamicArray() = default;

    template<typename U>
    void append(U&& value) {
        span_.append(std::forward<U>(value));
    }

    size_t size() const  {
        return span_.size();
    }

    bool empty() const  {
        return span_.empty();
    }

    UnqPtr<T> Get(size_t index) const {
        return span_.Get(index);
    }

    ShrdPtr<T> Copy(size_t index) const {
        return span_.Copy(index);
    }

    MsPtr<T> Locate(size_t index) {
        return span_.Locate(index);
    }

    MsPtr<T> begin() noexcept {
        return span_.begin();
    }

    MsPtr<T> end() noexcept {
        return span_.end();
    }
};

#endif