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
        check(s.size() == 0, "size() == 0 у пустого MemorySpan");
        check(s.empty(), "empty() == true у пустого MemorySpan");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём пустой MemorySpan. Проверяем, что size() == 0 и "
        "empty() == true. Внутри уже выделен небольшой буфер для будущих "
        "элементов, но логический размер — 0."
    );
}

inline void testMemorySpanAppend() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        check(s.size() == 3, "size() == 3 после трёх append");
        check(s.raw()[0] == 10, "первый элемент == 10");
        check(s.raw()[2] == 30, "третий элемент == 30");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем добавление элементов через append. Добавляем три "
        "числа — size() становится 3. Проверяем через raw()[], что "
        "элементы лежат в правильном порядке."
    );
}

inline void testMemorySpanGet() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(5);
        UnqPtr<int> p = s.Get(0);
        check(*p == 5, "*p == 5 (копия значения из спана)");
        p.reset(new int(99));
        check(*p == 99, "*p == 99 после reset (независимая копия)");
        check(s.raw()[0] == 5, "оригинал в спане не изменился (остался 5)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем MemorySpan::Get. Метод возвращает UnqPtr — это КОПИЯ "
        "элемента. Проверяем, что после изменения через полученный UnqPtr "
        "оригинал в спане не изменился. Это семантика копирования."
    );
}

inline void testMemorySpanCopy() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(7);
        ShrdPtr<int> p = s.Copy(0);
        check(*p == 7, "*p == 7 (значение скопировано)");
        ShrdPtr<int> q = p;
        check(p.use_count() == 2, "use_count == 2 после копирования ShrdPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем MemorySpan::Copy. Метод возвращает ShrdPtr — тоже "
        "КОПИЮ элемента, но с разделяемым владением. После ShrdPtr<int> "
        "q = p use_count == 2."
    );
}

inline void testMsPtrLocate() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(0);
        check(*p == 1, "*p == 1 (индекс 0)");
        MsPtr<int> q = s.Locate(2);
        check(*q == 3, "*q == 3 (индекс 2)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем MemorySpan::Locate и разыменование MsPtr. В отличие "
        "от Get и Copy, Locate НЕ копирует элемент — возвращает MsPtr, "
        "который указывает на элемент внутри буфера."
    );
}

inline void testMsPtrIncrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        MsPtr<int> p = s.Locate(0);
        check(*p == 10, "*p == 10 до ++");
        ++p;
        check(*p == 20, "*p == 20 после ++");
        ++p;
        check(*p == 30, "*p == 30 после второго ++");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем арифметику MsPtr — оператор ++. MsPtr работает как "
        "сырой указатель: ++ сдвигает индекс на следующий элемент."
    );
}

inline void testMsPtrDecrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(2);
        check(*p == 3, "*p == 3 (индекс 2)");
        --p;
        check(*p == 2, "*p == 2 после --");
        --p;
        check(*p == 1, "*p == 1 после второго --");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем оператор --. MsPtr позволяет двигаться в обе стороны. "
        "Начинаем с последнего элемента (индекс 2), уменьшаем индекс."
    );
}

inline void testMsPtrPostIncrement() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        MsPtr<int> p = s.Locate(0);
        MsPtr<int> old = p++;
        check(*old == 1, "old указывает на прежнее место (значение 1)");
        check(*p == 2, "p сдвинулся на следующий (значение 2)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем постфиксный ++. p++ возвращает КОПИЮ указателя на "
        "текущую позицию и затем сдвигает оригинал. old указывает на "
        "прежнее место (1), p уже на следующем (2)."
    );
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
        check(*q == 4, "*q == 4 (p + 3 == индекс 3)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем оператор + (сдвиг вправо без изменения оригинала). "
        "p указывает на индекс 0, p + 3 создаёт новый MsPtr на индекс 3. "
        "Проверяем, что q разыменовывается в 4."
    );
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
        check(*q == 2, "*q == 2 (p - 2 == индекс 1)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем оператор - (сдвиг влево). p на индексе 3, p - 2 — "
        "на индексе 1. Значение 2. Оригинал p не меняется."
    );
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
        check(*p == 3, "*p == 3 после += 2");
        p -= 1;
        check(*p == 2, "*p == 2 после -= 1");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем += и -=. Эти операторы ИЗМЕНЯЮТ сам указатель "
        "(в отличие от + и -). p начинается на индексе 0, += 2 → индекс 2, "
        "затем -= 1 → индекс 1."
    );
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
        check(diff == 3, "b - a == 3 (разность индексов)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем разность двух MsPtr. Как у сырых указателей, b - a "
        "даёт количество элементов между ними. b на индексе 3, a на 0, "
        "разность 3."
    );
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
        check(a == c, "a == c (один индекс)");
        check(a != b, "a != b (разные индексы)");
        check(a < b, "a < b");
        check(b > a, "b > a");
        check(a <= c, "a <= c");
        check(a >= c, "a >= c");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем все операторы сравнения MsPtr. Сравнение идёт по "
        "индексу (для указателей на один буфер). a и c равны, "
        "a < b, b > a и так далее."
    );
}

inline void testMsPtrIndexOperator() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(10);
        s.append(20);
        s.append(30);
        MsPtr<int> p = s.Locate(0);
        check(p[0] == 10, "p[0] == 10");
        check(p[1] == 20, "p[1] == 20");
        check(p[2] == 30, "p[2] == 30");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем оператор []. Работает как у сырого указателя: "
        "p[i] эквивалентно *(p + i). p сам на индексе 0, поэтому "
        "p[0], p[1], p[2] дают 10, 20, 30."
    );
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
        check(caught, "p[5] бросает std::out_of_range");
    } catch (...) {
        check(false, "тест упал с исключением вне try");
    }
    reportResult(
        "Проверяем защиту от выхода за границы. p[5] должен бросить "
        "std::out_of_range, потому что в спане только 2 элемента. "
        "Это главное преимущество MsPtr над сырым указателем."
    );
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
        check(caught, "*p на end() бросает std::out_of_range");
    } catch (...) {
        check(false, "тест упал с исключением вне try");
    }
    reportResult(
        "Проверяем разыменование end(). end() указывает на позицию "
        "после последнего элемента (индекс == size). Разыменовывать "
        "нельзя — должно быть std::out_of_range."
    );
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
        check(sum == 150, "сумма элементов == 150 (10+20+30+40+50)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем итерацию через MsPtr. MemorySpan::begin() и end() "
        "возвращают MsPtr на первый элемент и позицию после последнего. "
        "Классический цикл for: сумма 10+20+30+40+50 == 150."
    );
}

inline void testMsPtrBool() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        MsPtr<int> p = s.Locate(0);
        check(static_cast<bool>(p), "operator bool == true для валидного MsPtr");
        MsPtr<int> empty;
        check(!static_cast<bool>(empty), "operator bool == false для пустого MsPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем operator bool. MsPtr на живом буфере даёт true, "
        "MsPtr по умолчанию (без буфера) — false. Позволяет писать "
        "if (p) { ... } для проверки валидности."
    );
}

inline void testMsPtrIndexSize() {
    beginTest();
    try {
        MemorySpan<int> s;
        s.append(1);
        s.append(2);
        s.append(3);
        MsPtr<int> p = s.Locate(1);
        check(p.index() == 1, "index() == 1");
        check(p.size() == 3, "size() == 3");
        check(*p == 2, "*p == 2 (элемент по индексу 1)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем методы index() и size(). index() возвращает текущий "
        "индекс MsPtr в буфере, size() — размер буфера. p на индексе 1, "
        "размер 3."
    );
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
    testMsPtrIndexSize();
}

#endif