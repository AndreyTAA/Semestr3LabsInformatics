#ifndef HOMEWORK1_TESTSHRDPTR_H
#define HOMEWORK1_TESTSHRDPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "ShrdPtr.h"
#include "TestRunner.h"

struct ShrdTracked {
    static int alive;
    int value;
    ShrdTracked(int v) : value(v) { ++alive; }
    ~ShrdTracked() { --alive; }
};
int ShrdTracked::alive = 0;

struct ShrdBase {
    virtual ~ShrdBase() = default;
    int base = 10;
};
struct ShrdDerived : ShrdBase {
    int derived = 20;
};

inline void testShrdPtrDefault() {
    beginTest();
    try {
        ShrdPtr<int> p;
        check(p.get() == nullptr);
        check(!p);
        check(p.use_count() == 0);
    } catch (...) {
        check(false);
    }
    reportResult("Создание ShrdPtr по умолчанию");
}

inline void testShrdPtrFromRaw() {
    beginTest();
    try {
        auto p = makeShrd<int>(42);
        check(p.get() != nullptr);
        check(*p == 42);
        check(p.use_count() == 1);
    } catch (...) {
        check(false);
    }
    reportResult("Создание ShrdPtr через makeShrd, use_count == 1");
}

inline void testShrdPtrFromNullptr() {
    beginTest();
    try {
        ShrdPtr<int> p(nullptr);
        check(p.get() == nullptr);
        check(p.use_count() == 0);
    } catch (...) {
        check(false);
    }
    reportResult("Создание ShrdPtr из nullptr");
}

inline void testShrdPtrCopyCtor() {
    beginTest();
    try {
        auto a = makeShrd<int>(5);
        ShrdPtr<int> b(a);
        check(a.use_count() == 2);
        check(b.use_count() == 2);
        check(*a == *b);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: копирующий конструктор, use_count == 2");
}

inline void testShrdPtrCopyAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(1);
        auto b = makeShrd<int>(2);
        b = a;
        check(a.use_count() == 2);
        check(b.use_count() == 2);
        check(*b == 1);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: копирующее присваивание");
}

inline void testShrdPtrMoveCtor() {
    beginTest();
    try {
        auto a = makeShrd<int>(3);
        ShrdPtr<int> b(std::move(a));
        check(a.get() == nullptr);
        check(a.use_count() == 0);
        check(b.use_count() == 1);
        check(*b == 3);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: перемещающий конструктор");
}

inline void testShrdPtrMoveAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(4);
        auto b = makeShrd<int>(5);
        b = std::move(a);
        check(a.get() == nullptr);
        check(*b == 4);
        check(b.use_count() == 1);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: перемещающее присваивание");
}

inline void testShrdPtrSelfAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(7);
        ShrdPtr<int>& ref = a;
        a = ref;
        check(a.use_count() == 1);
        check(*a == 7);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: самоприсваивание");
}

inline void testShrdPtrUseCountDecrease() {
    beginTest();
    try {
        auto a = makeShrd<int>(5);
        {
            ShrdPtr<int> b(a);
            ShrdPtr<int> c(a);
            check(a.use_count() == 3);
        }
        check(a.use_count() == 1);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: use_count уменьшается при удалении копий");
}

inline void testShrdPtrDestructorDeletes() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b(a);
            check(ShrdTracked::alive == 1);
            check(a.use_count() == 2);
        }
        check(ShrdTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: объект удаляется при обнулении счётчика");
}

inline void testShrdPtrReset() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        auto a = makeShrd<ShrdTracked>(1);
        ShrdPtr<ShrdTracked> b(a);
        a.reset(new ShrdTracked(2));
        check(ShrdTracked::alive == 2);
        check(b.use_count() == 1);
        a.reset();
        check(ShrdTracked::alive == 1);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: reset с обнулением и без");
}

inline void testShrdPtrNoLeakAllReleased() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b = a;
            ShrdPtr<ShrdTracked> c = a;
            check(a.use_count() == 3);
        }
        check(ShrdTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: отсутствие утечек при удалении всех владельцев");
}

inline void testShrdPtrSubtyping() {
    beginTest();
    try {
        auto d = makeShrd<ShrdDerived>();
        ShrdPtr<ShrdBase> b(d);
        check(d.use_count() == 2);
        check(b.use_count() == 2);
        check(b->base == 10);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: подтипизация (Derived -> Base)");
}

inline void testShrdPtrMakeShrd() {
    beginTest();
    try {
        auto p = makeShrd<int>(123);
        check(*p == 123);
        check(p.use_count() == 1);
        auto q = makeShrd<std::string>("world");
        check(*q == "world");
    } catch (...) {
        check(false);
    }
    reportResult("makeShrd: фабрика ShrdPtr");
}

inline void testShrdPtrManyOwners() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        const int N = 100;
        {
            auto p = makeShrd<ShrdTracked>(0);
            ShrdPtr<ShrdTracked>* owners = new ShrdPtr<ShrdTracked>[N];
            for (int i = 0; i < N; ++i) {
                owners[i] = p;
            }
            check(p.use_count() == N + 1);
            delete[] owners;
            check(p.use_count() == 1);
            check(ShrdTracked::alive == 1);
        }
        check(ShrdTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: 100 владельцев одного объекта");
}

inline void testShrdPtrNoLeakAfterMove() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b(std::move(a));
            check(a.get() == nullptr);
            check(ShrdTracked::alive == 1);
        }
        check(ShrdTracked::alive == 0);
    } catch (...) {
        check(false);
    }
    reportResult("ShrdPtr: отсутствие утечек при перемещении");
}

inline void runAllShrdPtrTests() {
    std::cout << "\n===== Тесты ShrdPtr =====\n\n";
    testShrdPtrDefault();
    testShrdPtrFromRaw();
    testShrdPtrFromNullptr();
    testShrdPtrCopyCtor();
    testShrdPtrCopyAssign();
    testShrdPtrMoveCtor();
    testShrdPtrMoveAssign();
    testShrdPtrSelfAssign();
    testShrdPtrUseCountDecrease();
    testShrdPtrDestructorDeletes();
    testShrdPtrReset();
    testShrdPtrNoLeakAllReleased();
    testShrdPtrSubtyping();
    testShrdPtrMakeShrd();
    testShrdPtrManyOwners();
    testShrdPtrNoLeakAfterMove();
}

#endif