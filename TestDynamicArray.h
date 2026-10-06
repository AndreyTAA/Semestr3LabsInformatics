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
        check(arr.size() == 0, "size() == 0 у пустого DynamicArray");
        check(arr.empty(), "empty() == true у пустого DynamicArray");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём пустой DynamicArray. Проверяем, что size() == 0 и "
        "empty() == true. Внутри уже есть буфер для будущих элементов."
    );
}

inline void testDynArrayAppend() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(10);
        arr.append(20);
        arr.append(30);
        check(arr.size() == 3, "size() == 3 после трёх append");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем добавление элементов через append. После трёх "
        "append size() должен быть равен 3. Внутри вызывается "
        "MemorySpan::append, которая может расширять буфер."
    );
}

inline void testDynArrayGet() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(5);
        arr.append(15);
        UnqPtr<int> p = arr.Get(0);
        check(*p == 5, "*p == 5 (копия первого элемента)");
        p.reset(new int(99));
        check(*p == 99, "*p == 99 после reset (независимая копия)");
        UnqPtr<int> q = arr.Get(0);
        check(*q == 5, "оригинал в массиве не изменился (остался 5)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем DynamicArray::Get — возвращает UnqPtr, то есть "
        "КОПИЮ элемента. Изменения через этот UnqPtr не влияют на "
        "оригинал. Это демонстрирует safe-копирование через умные указатели."
    );
}

inline void testDynArrayCopy() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(7);
        ShrdPtr<int> a = arr.Copy(0);
        ShrdPtr<int> b = a;
        check(a.use_count() == 2, "use_count == 2 после копирования ShrdPtr");
        check(*b == 7, "*b == 7 (значение скопировано)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем DynamicArray::Copy — возвращает ShrdPtr. Два указателя "
        "a и b разделяют владение одной копией элемента. use_count == 2."
    );
}

inline void testDynArrayLocate() {
    beginTest();
    try {
        DynamicArray<int> arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        MsPtr<int> p = arr.Locate(0);
        check(*p == 1, "*p == 1 (индекс 0)");
        ++p;
        check(*p == 2, "*p == 2 после ++");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем DynamicArray::Locate — возвращает MsPtr. MsPtr "
        "работает с арифметикой указателей (++). Проверяем, что "
        "можно пройти по элементам массива через него."
    );
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
        check(sum == 15, "сумма элементов == 15 (1+2+3+4+5)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем итерацию по DynamicArray через MsPtr. Методы "
        "begin() и end() возвращают MsPtr. Обычный цикл for собирает "
        "сумму всех элементов. Это демонстрирует, что MsPtr может "
        "использоваться как итератор."
    );
}

inline void testDynArrayOfStrings() {
    beginTest();
    try {
        DynamicArray<std::string> arr;
        arr.append("alpha");
        arr.append("beta");
        arr.append("gamma");
        check(arr.size() == 3, "size() == 3 для строкового массива");
        UnqPtr<std::string> p = arr.Get(1);
        check(*p == "beta", "*p == \"beta\"");
        MsPtr<std::string> q = arr.Locate(2);
        check(*q == "gamma", "*q == \"gamma\"");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем DynamicArray со строками. Умные указатели работают "
        "с любым T, не только с POD. Проверяем Get (копия) и Locate "
        "(арифметический указатель) для std::string."
    );
}

inline void testDynArrayOfUnqPtr() {
    beginTest();
    try {
        DynamicArray<UnqPtr<int>> arr;
        arr.append(makeUnq<int>(1));
        arr.append(makeUnq<int>(2));
        check(arr.size() == 2, "size() == 2 после двух append UnqPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем DynamicArray из UnqPtr. Это нетривиальный случай: "
        "UnqPtr нельзя копировать, только перемещать. MemorySpan::append "
        "использует forwarding и move-assignment, чтобы это сработало."
    );
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