#ifndef HOMEWORK1_CONTROLBLOCK_H
#define HOMEWORK1_CONTROLBLOCK_H

#include <cstddef>

class ControlBlock {
private:
    void* ptr_;
    size_t strong_;
    size_t weak_;
    void (*deleter_)(void*);

    ControlBlock(void* ptr, void (*deleter)(void*));

public:
    template<typename T>
    static ControlBlock* makeSingle(T* ptr);

    template<typename T>
    static ControlBlock* makeArray(T* ptr);

    void incStrong();
    void decStrong();
    void incWeak();
    void decWeak();

    void* get() const;
    bool expired() const;
    size_t useCount() const ;
};

inline ControlBlock::ControlBlock(void* ptr, void (*deleter)(void*))
    : ptr_(ptr), strong_(1), weak_(1), deleter_(deleter) {}

template<typename T>
ControlBlock* ControlBlock::makeSingle(T* ptr) {
    return new ControlBlock(
        ptr,
        [](void* p) { delete static_cast<T*>(p); });
}

template<typename T>
ControlBlock* ControlBlock::makeArray(T* ptr) {
    return new ControlBlock(
        ptr,
        [](void* p) { delete[] static_cast<T*>(p); });
}

inline void ControlBlock::incStrong()  {
    ++strong_;
    ++weak_;
}

inline void ControlBlock::decStrong()  {
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

inline void ControlBlock::incWeak()  {
    ++weak_;
}

inline void ControlBlock::decWeak()  {
    if (--weak_ == 0 && strong_ == 0) {
        delete this;
    }
}

inline void* ControlBlock::get() const  {
    return ptr_;
}

inline bool ControlBlock::expired() const  {
    return strong_ == 0;
}

inline size_t ControlBlock::useCount() const  {
    return strong_;
}

#endif // HOMEWORK1_CONTROLBLOCK_H