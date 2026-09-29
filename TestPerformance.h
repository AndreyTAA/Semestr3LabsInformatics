#ifndef HOMEWORK1_TESTPERFORMANCE_H
#define HOMEWORK1_TESTPERFORMANCE_H

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "WeakPtr.h"

struct PerfObject {
    int value;
    double data[4];
    PerfObject(int v) : value(v) {
        for (int i = 0; i < 4; ++i) data[i] = v * 1.5;
    }
};

using PerfClock = std::chrono::high_resolution_clock;
using PerfMs = std::chrono::milliseconds;

inline long long measureCreateRaw(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        PerfObject* p = new PerfObject(i);
        delete p;
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateUnq(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        auto p = makeUnq<PerfObject>(i);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateShrd(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        auto p = makeShrd<PerfObject>(i);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateStdUnq(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        std::unique_ptr<PerfObject> p(new PerfObject(i));
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateStdShrd(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        std::shared_ptr<PerfObject> p(new PerfObject(i));
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateWeak(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        auto s = makeShrd<PerfObject>(i);
        WeakPtr<PerfObject> w(s);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCreateStdWeak(int n) {
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        std::shared_ptr<PerfObject> s(new PerfObject(i));
        std::weak_ptr<PerfObject> w(s);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCopyShrd(int n) {
    auto p = makeShrd<PerfObject>(0);
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        ShrdPtr<PerfObject> q(p);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureCopyStdShrd(int n) {
    std::shared_ptr<PerfObject> p(new PerfObject(0));
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        std::shared_ptr<PerfObject> q(p);
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureLockWeak(int n) {
    auto s = makeShrd<PerfObject>(0);
    WeakPtr<PerfObject> w(s);
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        ShrdPtr<PerfObject> locked = w.lock();
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline long long measureLockStdWeak(int n) {
    std::shared_ptr<PerfObject> s(new PerfObject(0));
    std::weak_ptr<PerfObject> w(s);
    auto start = PerfClock::now();
    for (int i = 0; i < n; ++i) {
        std::shared_ptr<PerfObject> locked = w.lock();
    }
    return std::chrono::duration_cast<PerfMs>(PerfClock::now() - start).count();
}

inline void drawTimeBar(const std::string& label, long long value, long long maxValue) {
    int width = 40;
    int bars = maxValue > 0 ? static_cast<int>(static_cast<double>(value) / maxValue * width) : 0;
    std::cout << std::setw(18) << label << " | ";
    for (int i = 0; i < bars; ++i) std::cout << "#";
    for (int i = bars; i < width; ++i) std::cout << " ";
    std::cout << " | " << value << " ms\n";
}

inline void graphCreation(int n, long long raw, long long unq, long long shrd,
                          long long stdUnq, long long stdShrd) {
    long long maxVal = std::max({raw, unq, shrd, stdUnq, stdShrd});
    std::cout << "\nГрафик: создание и удаление " << n << " объектов\n";
    drawTimeBar("raw", raw, maxVal);
    drawTimeBar("UnqPtr", unq, maxVal);
    drawTimeBar("ShrdPtr", shrd, maxVal);
    drawTimeBar("std::unique_ptr", stdUnq, maxVal);
    drawTimeBar("std::shared_ptr", stdShrd, maxVal);
}

inline void graphCopy(int n, long long shrd, long long stdShrd) {
    long long maxVal = std::max({shrd, stdShrd});
    std::cout << "\nГрафик: копирование разделяемого указателя " << n << " раз\n";
    drawTimeBar("ShrdPtr", shrd, maxVal);
    drawTimeBar("std::shared_ptr", stdShrd, maxVal);
}

inline void graphWeakCreation(int n, long long weak, long long stdWeak) {
    long long maxVal = std::max({weak, stdWeak});
    std::cout << "\nГрафик: создание WeakPtr " << n << " раз\n";
    drawTimeBar("WeakPtr", weak, maxVal);
    drawTimeBar("std::weak_ptr", stdWeak, maxVal);
}

inline void graphWeakLock(int n, long long lock, long long stdLock) {
    long long maxVal = std::max({lock, stdLock});
    std::cout << "\nГрафик: WeakPtr::lock " << n << " раз\n";
    drawTimeBar("WeakPtr::lock", lock, maxVal);
    drawTimeBar("std::weak_ptr::lock", stdLock, maxVal);
}

inline void printPerfTableHeader() {
    std::cout << std::setw(10) << "N"
              << std::setw(12) << "raw"
              << std::setw(12) << "UnqPtr"
              << std::setw(12) << "ShrdPtr"
              << std::setw(16) << "std::unique"
              << std::setw(16) << "std::shared"
              << "\n";
    std::cout << std::string(80, '-') << "\n";
}

inline void printCopyTableHeader() {
    std::cout << std::setw(10) << "N"
              << std::setw(14) << "ShrdPtr"
              << std::setw(18) << "std::shared_ptr"
              << "\n";
    std::cout << std::string(44, '-') << "\n";
}

inline void printWeakTableHeader() {
    std::cout << std::setw(10) << "N"
              << std::setw(14) << "WeakPtr"
              << std::setw(18) << "std::weak_ptr"
              << "\n";
    std::cout << std::string(44, '-') << "\n";
}

inline void printWeakLockTableHeader() {
    std::cout << std::setw(10) << "N"
              << std::setw(16) << "WeakPtr::lock"
              << std::setw(20) << "std::weak_ptr::lock"
              << "\n";
    std::cout << std::string(48, '-') << "\n";
}

inline void runPerformanceTests() {
    std::cout << "\n=====================================\n";
    std::cout << "    Тесты производительности\n";
    std::cout << "=====================================\n";

    std::vector<int> sizes = {100, 1000, 10000, 100000, 1000000};

    std::cout << "\n--- Таблица: создание и удаление объектов (ms) ---\n";
    printPerfTableHeader();

    for (int n : sizes) {
        long long raw = measureCreateRaw(n);
        long long unq = measureCreateUnq(n);
        long long shrd = measureCreateShrd(n);
        long long stdUnq = measureCreateStdUnq(n);
        long long stdShrd = measureCreateStdShrd(n);

        std::cout << std::setw(10) << n
                  << std::setw(12) << raw
                  << std::setw(12) << unq
                  << std::setw(12) << shrd
                  << std::setw(16) << stdUnq
                  << std::setw(16) << stdShrd
                  << "\n";
    }

    std::cout << "\n--- Таблица: копирование разделяемых указателей (ms) ---\n";
    printCopyTableHeader();

    for (int n : sizes) {
        long long shrd = measureCopyShrd(n);
        long long stdShrd = measureCopyStdShrd(n);

        std::cout << std::setw(10) << n
                  << std::setw(14) << shrd
                  << std::setw(18) << stdShrd
                  << "\n";
    }

    std::cout << "\n--- Таблица: создание WeakPtr вместе с ShrdPtr (ms) ---\n";
    printWeakTableHeader();

    for (int n : sizes) {
        long long weak = measureCreateWeak(n);
        long long stdWeak = measureCreateStdWeak(n);

        std::cout << std::setw(10) << n
                  << std::setw(14) << weak
                  << std::setw(18) << stdWeak
                  << "\n";
    }

    std::cout << "\n--- Таблица: WeakPtr::lock (ms) ---\n";
    printWeakLockTableHeader();

    for (int n : sizes) {
        long long lock = measureLockWeak(n);
        long long stdLock = measureLockStdWeak(n);

        std::cout << std::setw(10) << n
                  << std::setw(16) << lock
                  << std::setw(20) << stdLock
                  << "\n";
    }

    int n1 = 100000;
    graphCreation(n1,
        measureCreateRaw(n1),
        measureCreateUnq(n1),
        measureCreateShrd(n1),
        measureCreateStdUnq(n1),
        measureCreateStdShrd(n1));

    graphCopy(n1,
        measureCopyShrd(n1),
        measureCopyStdShrd(n1));

    graphWeakCreation(n1,
        measureCreateWeak(n1),
        measureCreateStdWeak(n1));

    graphWeakLock(n1,
        measureLockWeak(n1),
        measureLockStdWeak(n1));

    int n2 = 1000000;
    graphCreation(n2,
        measureCreateRaw(n2),
        measureCreateUnq(n2),
        measureCreateShrd(n2),
        measureCreateStdUnq(n2),
        measureCreateStdShrd(n2));

    graphCopy(n2,
        measureCopyShrd(n2),
        measureCopyStdShrd(n2));

    graphWeakCreation(n2,
        measureCreateWeak(n2),
        measureCreateStdWeak(n2));

    graphWeakLock(n2,
        measureLockWeak(n2),
        measureLockStdWeak(n2));

    std::cout << "\nТесты производительности завершены.\n";
}

#endif