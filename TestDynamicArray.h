#ifndef HOMEWORK1_TESTDYNAMICARRAY_H
#define HOMEWORK1_TESTDYNAMICARRAY_H

#include <iostream>
#include <string>
#include "DynamicArray.h"
#include "TestRunner.h"

inline void testDynArrayEmpty() {
    beginTest();
    try {
        DynamicArray<int> arr;
        check(arr.size() == 0);
        check(arr.empty());
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray: пустой контейнер");
}

inline void testDynArrayAppend() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(10);
        arr.append(20);
        arr.append(30);
        check(arr.size() == 3);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray: добавление элементов");
}

inline void testDynArrayGet() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(5);
        arr.append(15);
        UnqPtr<int> p = arr.Get(0);
        check(*p == 5);
        p.reset(new int(99));
        check(*p == 99);
        UnqPtr<int> q = arr.Get(0);
        check(*q == 5);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray::Get возвращает UnqPtr (независимую копию)");
}

inline void testDynArrayCopy() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(7);
        ShrdPtr<int> a = arr.Copy(0);
        ShrdPtr<int> b = a;
        check(a.use_count() == 2);
        check(*b == 7);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray::Copy возвращает ShrdPtr");
}

inline void testDynArrayLocate() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        MsPtr<int> p = arr.Locate(0);
        check(*p == 1);
        ++p;
        check(*p == 2);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray::Locate возвращает MsPtr");
}

inline void testDynArrayIteration() {
    beginTest();
    try {
        DynamicArray<int> arr;
        for (int i = 1; i <= 5; ++i) arr.append(i);
        int sum = 0;
        for (MsPtr<int> p = arr.begin(); p != arr.end(); ++p) {
            sum += *p;
        }
        check(sum == 15);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray: итерация через MsPtr (сумма == 15)");
}

inline void testDynArrayOfStrings() {
    beginTest();
    try {
        DynamicArray<std::string> arr;
        arr.append("alpha");
        arr.append("beta");
        arr.append("gamma");
        check(arr.size() == 3);
        UnqPtr<std::string> p = arr.Get(1);
        check(*p == "beta");
        MsPtr<std::string> q = arr.Locate(2);
        check(*q == "gamma");
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray: строковые элементы");
}

inline void testDynArrayOfUnqPtr() {
    beginTest();
    try {
        DynamicArray<UnqPtr<int>> arr;
        arr.append(makeUnq<int>(1));
        arr.append(makeUnq<int>(2));
        check(arr.size() == 2);
    } catch (...) {
        check(false);
    }
    reportResult("DynamicArray: элементы типа UnqPtr");
}

inline void runAllDynamicArrayTests() {
    std::cout << "\n===== Тесты DynamicArray =====\n\n";
    testDynArrayEmpty();
    testDynArrayAppend();
    testDynArrayGet();
    testDynArrayCopy();
    testDynArrayLocate();
    testDynArrayIteration();
    testDynArrayOfStrings();
    testDynArrayOfUnqPtr();
}

#endif