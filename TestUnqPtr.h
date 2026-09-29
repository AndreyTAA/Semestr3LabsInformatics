#ifndef HOMEWORK1_TESTUNQPTR_H
#define HOMEWORK1_TESTUNQPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "UnqPtr.h"
#include "TestRunner.h"

struct UnqTracked {
    static int alive;
    int value;
    UnqTracked(int v) : value(v) { ++alive; }
    ~UnqTracked() { --alive; }
    UnqTracked(const UnqTracked&) = delete;
    UnqTracked& operator=(const UnqTracked&) = delete;
};
int UnqTracked::alive = 0;

struct UnqBase {
    virtual ~UnqBase() = default;
    int base = 1;
};
struct UnqDerived : UnqBase {
    int derived = 2;
};

inline void testUnqPtrDefault() {
    beginTest();
    try {
        UnqPtr<int> p;
        check(p.get() == nullptr);
        check(!p);
    } catch (...) {
        check(false);
    }
    reportResult("Создание UnqPtr по умолчанию (пустой указатель)");
}

inline void testUnqPtrFromRaw() {
    beginTest();
    try {
        auto p = makeUnq<int>(42);
        check(p.get() != nullptr);
        check(*p == 42);
        check(static_cast<bool>(p));
    } catch (...) {
        check(false);
    }
    reportResult("Создание UnqPtr через makeUnq");
}

inline void testUnqPtrFromNullptr() {
    beginTest();
    try {
        UnqPtr<int> p(nullptr);
        check(p.get() == nullptr);
        check(!p);
    } catch (...) {
        check(false);
    }
    reportResult("Создание UnqPtr из nullptr");
}

inline void testUnqPtrMoveCtor() {
    beginTest();
    try {
        auto a = makeUnq<int>(10);
        UnqPtr<int> b(std::move(a));
        check(a.get() == nullptr);
        check(b.get() != nullptr);
        check(*b == 10);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: перемещающий конструктор");
}

inline void testUnqPtrMoveAssign() {
    beginTest();
    try {
        auto a = makeUnq<int>(1);
        auto b = makeUnq<int>(2);
        b = std::move(a);
        check(a.get() == nullptr);
        check(*b == 1);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: перемещающее присваивание");
}

inline void testUnqPtrMoveAssignToSelf() {
    beginTest();
    try {
        auto a = makeUnq<int>(5);
        int* raw = a.get();
        a = std::move(a);
        check(a.get() == raw);
        check(*a == 5);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: самоперемещение");
}

inline void testUnqPtrRelease() {
    beginTest();
    try {
        auto p = makeUnq<int>(7);
        int* raw = p.release();
        check(p.get() == nullptr);
        check(*raw == 7);
        delete raw;
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: release отдаёт владение и обнуляет указатель");
}

inline void testUnqPtrReset() {
    beginTest();
    try {
        auto p = makeUnq<int>(1);
        int* old = p.get();
        p.reset(new int(2));
        check(p.get() != old);
        check(*p == 2);
        p.reset();
        check(p.get() == nullptr);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: reset удаляет старый и берёт новый объект");
}

inline void testUnqPtrResetSame() {
    beginTest();
    try {
        auto p = makeUnq<int>(3);
        int* raw = p.get();
        p.reset(raw);
        check(p.get() == raw);
        check(*p == 3);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: reset тем же указателем не удаляет объект");
}

inline void testUnqPtrSwap() {
    beginTest();
    try {
        auto a = makeUnq<int>(1);
        auto b = makeUnq<int>(2);
        a.swap(b);
        check(*a == 2);
        check(*b == 1);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: swap обменивает владение");
}

inline void testUnqPtrDestructorDeletes() {
    beginTest();
    try {
        UnqTracked::alive = 0;
        {
            auto p = makeUnq<UnqTracked>(5);
            check(UnqTracked::alive == 1);
        }
        check(UnqTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: деструктор удаляет объект");
}

inline void testUnqPtrDerived() {
    beginTest();
    try {
        UnqPtr<UnqBase> p = makeUnq<UnqDerived>();
        check(p->base == 1);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: хранит производный объект через базовый указатель");
}

inline void testUnqPtrSubtyping() {
    beginTest();
    try {
        auto d = makeUnq<UnqDerived>();
        UnqPtr<UnqBase> b(std::move(d));
        check(d.get() == nullptr);
        check(b.get() != nullptr);
        check(b->base == 1);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: подтипизация (Derived -> Base) при перемещении");
}

inline void testUnqPtrMakeUnq() {
    beginTest();
    try {
        auto p = makeUnq<int>(99);
        check(*p == 99);
        auto q = makeUnq<std::string>("hello");
        check(*q == "hello");
    } catch (...) {
        check(false);
    }
    reportResult("makeUnq: фабрика уникальных указателей");
}

inline void testUnqPtrMultipleAllocs() {
    beginTest();
    try {
        const int N = 1000;
        UnqTracked::alive = 0;
        {
            UnqPtr<UnqTracked>* arr = new UnqPtr<UnqTracked>[N];
            for (int i = 0; i < N; ++i) {
                arr[i] = makeUnq<UnqTracked>(i);
            }
            check(UnqTracked::alive == N);
            delete[] arr;
        }
        check(UnqTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: массовые аллокации (1000 объектов) и корректное удаление");
}

inline void testUnqPtrNoLeakOnException() {
    beginTest();
    try {
        UnqTracked::alive = 0;
        try {
            auto p = makeUnq<UnqTracked>(1);
            throw std::runtime_error("test");
        } catch (...) {}
        check(UnqTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("UnqPtr: отсутствие утечек при выбросе исключения");
}

inline void runAllUnqPtrTests() {
    std::cout << "\n===== Тесты UnqPtr =====\n\n";
    testUnqPtrDefault();
    testUnqPtrFromRaw();
    testUnqPtrFromNullptr();
    testUnqPtrMoveCtor();
    testUnqPtrMoveAssign();
    testUnqPtrMoveAssignToSelf();
    testUnqPtrRelease();
    testUnqPtrReset();
    testUnqPtrResetSame();
    testUnqPtrSwap();
    testUnqPtrDestructorDeletes();
    testUnqPtrDerived();
    testUnqPtrSubtyping();
    testUnqPtrMakeUnq();
    testUnqPtrMultipleAllocs();
    testUnqPtrNoLeakOnException();
}

#endif