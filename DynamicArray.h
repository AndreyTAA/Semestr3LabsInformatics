#ifndef HOMEWORK1_DYNAMICARRAY_H
#define HOMEWORK1_DYNAMICARRAY_H

#include <cstddef>
#include <utility>
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
    void append(U&& value);

    size_t size() const;
    bool empty() const;

    UnqPtr<T> Get(size_t index) const;
    ShrdPtr<T> Copy(size_t index) const;
    MsPtr<T> Locate(size_t index);

    MsPtr<T> begin() ;
    MsPtr<T> end() ;
};

template<typename T>
template<typename U>
void DynamicArray<T>::append(U&& value) {
    span_.append(std::forward<U>(value));
}

template<typename T>
size_t DynamicArray<T>::size() const {
    return span_.size();
}

template<typename T>
bool DynamicArray<T>::empty() const {
    return span_.empty();
}

template<typename T>
UnqPtr<T> DynamicArray<T>::Get(size_t index) const {
    return span_.Get(index);
}

template<typename T>
ShrdPtr<T> DynamicArray<T>::Copy(size_t index) const {
    return span_.Copy(index);
}

template<typename T>
MsPtr<T> DynamicArray<T>::Locate(size_t index) {
    return span_.Locate(index);
}

template<typename T>
MsPtr<T> DynamicArray<T>::begin()  {
    return span_.begin();
}

template<typename T>
MsPtr<T> DynamicArray<T>::end()  {
    return span_.end();
}

#endif // HOMEWORK1_DYNAMICARRAY_H