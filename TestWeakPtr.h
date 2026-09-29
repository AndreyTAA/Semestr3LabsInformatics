#ifndef HOMEWORK1_TESTWEAKPTR_H
#define HOMEWORK1_TESTWEAKPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "ShrdPtr.h"
#include "WeakPtr.h"
#include "TestRunner.h"

struct WeakTracked {
    static int alive;
    int value;
    WeakTracked(int v) : value(v) { ++alive; }
    ~WeakTracked() { --alive; }
};
int WeakTracked::alive = 0;

struct WeakBase {
    virtual ~WeakBase() = default;
    int base = 10;
};
struct WeakDerived : WeakBase {
    int derived = 20;
};

inline void testWeakPtrDefault() {
    beginTest();
    try {
        WeakPtr<int> w;
        check(w.expired());
        check(w.use_count() == 0);
        check(w.lock().get() == nullptr);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: конструктор по умолчанию (пустой, expired)");
}

inline void testWeakPtrFromShrd() {
    beginTest();
    try {
        auto s = makeShrd<int>(42);
        WeakPtr<int> w(s);
        check(!w.expired());
        check(w.use_count() == 1);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: создание из ShrdPtr (не expired, use_count == 1)");
}

inline void testWeakPtrLockAlive() {
    beginTest();
    try {
        auto s = makeShrd<int>(42);
        WeakPtr<int> w(s);
        ShrdPtr<int> locked = w.lock();
        check(locked.get() != nullptr);
        check(*locked == 42);
        check(s.use_count() == 2);
        check(w.use_count() == 2);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr::lock: возвращает живой ShrdPtr, когда объект жив");
}

inline void testWeakPtrLockExpired() {
    beginTest();
    try {
        WeakPtr<int> w;
        {
            auto s = makeShrd<int>(42);
            w = s;
            check(!w.expired());
        }
        check(w.expired());
        ShrdPtr<int> locked = w.lock();
        check(locked.get() == nullptr);
        check(w.use_count() == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr::lock: возвращает пустой ShrdPtr, когда объект мёртв");
}

inline void testWeakPtrCopyCtor() {
    beginTest();
    try {
        auto s = makeShrd<int>(5);
        WeakPtr<int> w1(s);
        WeakPtr<int> w2(w1);
        check(!w1.expired());
        check(!w2.expired());
        check(w1.use_count() == 1);
        check(w2.use_count() == 1);
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(s.use_count() == 3);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: копирующий конструктор (обе ссылки на тот же объект)");
}

inline void testWeakPtrCopyAssign() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w2 = w1;
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(l1.get() == l2.get());
        check(*l1 == 1);
        check(*l2 == 1);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: копирующее присваивание");
}

inline void testWeakPtrMoveCtor() {
    beginTest();
    try {
        auto s = makeShrd<int>(7);
        WeakPtr<int> w1(s);
        WeakPtr<int> w2(std::move(w1));
        check(!w2.expired());
        check(w1.expired());
        auto l = w2.lock();
        check(l.get() != nullptr);
        check(*l == 7);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: перемещающий конструктор");
}

inline void testWeakPtrMoveAssign() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w2 = std::move(w1);
        check(!w2.expired());
        check(w1.expired());
        auto l = w2.lock();
        check(*l == 1);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: перемещающее присваивание");
}

inline void testWeakPtrAssignFromShrd() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w(s1);
        w = s2;
        auto l = w.lock();
        check(l.get() != nullptr);
        check(*l == 2);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: присваивание из ShrdPtr");
}

inline void testWeakPtrReset() {
    beginTest();
    try {
        auto s = makeShrd<int>(1);
        WeakPtr<int> w(s);
        check(!w.expired());
        w.reset();
        check(w.expired());
        check(w.use_count() == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr::reset: сбрасывает слабую ссылку");
}

inline void testWeakPtrSwap() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w1.swap(w2);
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(*l1 == 2);
        check(*l2 == 1);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr::swap: обмен слабыми ссылками");
}

inline void testWeakPtrDoesNotDelete() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w(s);
            check(WeakTracked::alive == 1);
        }
        check(WeakTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: не удерживает объект живым (объект удаляется с ShrdPtr)");
}

inline void testWeakPtrKeepsBlockAlive() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        WeakPtr<WeakTracked> w;
        {
            auto s = makeShrd<WeakTracked>(1);
            w = s;
            check(!w.expired());
        }
        check(WeakTracked::alive == 0);
        check(w.expired());
        check(w.lock().get() == nullptr);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: блок жив после смерти объекта, expired == true");
}

inline void testWeakPtrLockExtendsLife() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w(s);

            ShrdPtr<WeakTracked> locked = w.lock();
            check(locked.get() != nullptr);
            check(WeakTracked::alive == 1);

            s.reset();
            check(WeakTracked::alive == 1);
            check(locked.use_count() == 1);
            check(w.use_count() == 1);
            check(!w.expired());
        }
        check(WeakTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr::lock: продлевает жизнь объекта через ShrdPtr");
}

inline void testWeakPtrManyWeak() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        const int N = 100;
        {
            auto s = makeShrd<WeakTracked>(0);
            WeakPtr<WeakTracked>* arr = new WeakPtr<WeakTracked>[N];
            for (int i = 0; i < N; ++i) {
                arr[i] = s;
            }
            check(s.use_count() == 1);
            check(WeakTracked::alive == 1);
            delete[] arr;
            check(s.use_count() == 1);
            check(WeakTracked::alive == 1);
        }
        check(WeakTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: 100 слабых ссылок не влияют на сильный счётчик");
}

inline void testWeakPtrCycle() {
    beginTest();
    try {
        struct Node {
            ShrdPtr<Node> next;
            WeakPtr<Node> prev;
            int value;
            Node(int v) : value(v) {}
        };

        auto a = makeShrd<Node>(1);
        auto b = makeShrd<Node>(2);
        a->next = b;
        b->prev = a;

        check(a.use_count() == 1);
        check(b.use_count() == 2);

        a.reset();
        check(b->prev.expired());
        b.reset();
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: разрыв циклической ссылки (классический сценарий)");
}

inline void testWeakPtrNoLeak() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w1(s);
            WeakPtr<WeakTracked> w2 = w1;
            WeakPtr<WeakTracked> w3 = w2;
        }
        check(WeakTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("WeakPtr: отсутствие утечек при множестве слабых ссылок");
}

inline void runAllWeakPtrTests() {
    std::cout << "\n===== Тесты WeakPtr =====\n\n";
    testWeakPtrDefault();
    testWeakPtrFromShrd();
    testWeakPtrLockAlive();
    testWeakPtrLockExpired();
    testWeakPtrCopyCtor();
    testWeakPtrCopyAssign();
    testWeakPtrMoveCtor();
    testWeakPtrMoveAssign();
    testWeakPtrAssignFromShrd();
    testWeakPtrReset();
    testWeakPtrSwap();
    testWeakPtrDoesNotDelete();
    testWeakPtrKeepsBlockAlive();
    testWeakPtrLockExtendsLife();
    testWeakPtrManyWeak();
    testWeakPtrCycle();
    testWeakPtrNoLeak();
}

#endif