#ifndef HOMEWORK1_TESTMSPTR_H
#define HOMEWORK1_TESTMSPTR_H

#include <iostream>
#include <stdexcept>
#include "MemorySpan.h"
#include "MsPtr.h"
#include "TestRunner.h"

inline void testMemorySpanEmpty() {
    beginTest();
    try {
        MemorySpan<int> s;
        check(s.size() == 0);
        check(s.empty());
    } catch (...) {
        check(false);
    }
    reportResult("MemorySpan: пустой контейнер");
}

inline void testMemorySpanAppend() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        check(s.size() == 3);
        check(s.raw()[0] == 10);
        check(s.raw()[2] == 30);
    } catch (...) {
        check(false);
    }
    reportResult("MemorySpan: добавление элементов");
}

inline void testMemorySpanGet() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(5);
        UnqPtr<int> p = s.Get(0);
        check(*p == 5);
        p.reset(new int(99));
        check(*p == 99);
        check(s.raw()[0] == 5);
    } catch (...) {
        check(false);
    }
    reportResult("MemorySpan::Get возвращает независимый UnqPtr");
}

inline void testMemorySpanCopy() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(7);
        ShrdPtr<int> p = s.Copy(0);
        check(*p == 7);
        ShrdPtr<int> q = p;
        check(p.use_count() == 2);
    } catch (...) {
        check(false);
    }
    reportResult("MemorySpan::Copy возвращает ShrdPtr");
}

inline void testMsPtrLocate() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(0);
        check(*p == 1);
        MsPtr<int> q = s.Locate(2);
        check(*q == 3);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: Locate и разыменование");
}

inline void testMsPtrIncrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        MsPtr<int> p = s.Locate(0);
        check(*p == 10);
        ++p;
        check(*p == 20);
        ++p;
        check(*p == 30);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор ++ (префиксный)");
}

inline void testMsPtrDecrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(2);
        check(*p == 3);
        --p;
        check(*p == 2);
        --p;
        check(*p == 1);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор -- (префиксный)");
}

inline void testMsPtrPostIncrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        MsPtr<int> p = s.Locate(0);
        MsPtr<int> old = p++;
        check(*old == 1);
        check(*p == 2);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор ++ (постфиксный)");
}

inline void testMsPtrAddition() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        s.append(4);
        MsPtr<int> p = s.Locate(0);
        MsPtr<int> q = p + 3;
        check(*q == 4);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор + (сдвиг вправо)");
}

inline void testMsPtrSubtraction() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        s.append(4);
        MsPtr<int> p = s.Locate(3);
        MsPtr<int> q = p - 2;
        check(*q == 2);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор - (сдвиг влево)");
}

inline void testMsPtrCompoundAdd() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        s.append(4);
        MsPtr<int> p = s.Locate(0);
        p += 2;
        check(*p == 3);
        p -= 1;
        check(*p == 2);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: операторы += и -=");
}

inline void testMsPtrDifference() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        s.append(4);
        MsPtr<int> a = s.Locate(0);
        MsPtr<int> b = s.Locate(3);
        ptrdiff_t diff = b - a;
        check(diff == 3);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: разность указателей");
}

inline void testMsPtrComparison() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> a = s.Locate(0);
        MsPtr<int> b = s.Locate(1);
        MsPtr<int> c = s.Locate(0);
        check(a == c);
        check(a != b);
        check(a < b);
        check(b > a);
        check(a <= c);
        check(a >= c);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: операторы сравнения");
}

inline void testMsPtrIndexOperator() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        MsPtr<int> p = s.Locate(0);
        check(p[0] == 10);
        check(p[1] == 20);
        check(p[2] == 30);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор []");
}

inline void testMsPtrBounds() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        MsPtr<int> p = s.Locate(0);
        bool caught = false;
        try {
            (void)p[5];
        } catch (const std::out_of_range&) {
            caught = true;
        }
        check(caught);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: выход за границы бросает исключение");
}

inline void testMsPtrDerefEnd() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        MsPtr<int> p = s.end();
        bool caught = false;
        try {
            (void)*p;
        } catch (const std::out_of_range&) {
            caught = true;
        }
        check(caught);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: разыменование end() бросает исключение");
}

inline void testMsPtrRangeIteration() {
    beginTest();
    try {
        MemorySpan<int> s;
        for (int i = 1; i <= 5; ++i) s.append(i * 10);
        int sum = 0;
        for (MsPtr<int> p = s.begin(); p != s.end(); ++p) {
            sum += *p;
        }
        check(sum == 150);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: итерация по диапазону (сумма == 150)");
}

inline void testMsPtrBool() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        MsPtr<int> p = s.Locate(0);
        check(static_cast<bool>(p));
        MsPtr<int> empty;
        check(!static_cast<bool>(empty));
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: оператор bool");
}

inline void testMsPtrIndex() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(1);
        check(p.index() == 1);
        p += 2;
        check(p.index() == 3);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: метод index()");
}

inline void testMsPtrOwner() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(1);
        check(p.index() == 1);
        check(p.size() == 3);
        check(*p == 2);
    } catch (...) {
        check(false);
    }
    reportResult("MsPtr: index() и size()");
}

inline void runAllMsPtrTests() {
    std::cout << "\n===== Тесты MemorySpan и MsPtr =====\n\n";
    testMemorySpanEmpty();
    testMemorySpanAppend();
    testMemorySpanGet();
    testMemorySpanCopy();
    testMsPtrLocate();
    testMsPtrIncrement();
    testMsPtrDecrement();
    testMsPtrPostIncrement();
    testMsPtrAddition();
    testMsPtrSubtraction();
    testMsPtrCompoundAdd();
    testMsPtrDifference();
    testMsPtrComparison();
    testMsPtrIndexOperator();
    testMsPtrBounds();
    testMsPtrDerefEnd();
    testMsPtrRangeIteration();
    testMsPtrBool();
    testMsPtrIndex();
    testMsPtrOwner();
}

#endif