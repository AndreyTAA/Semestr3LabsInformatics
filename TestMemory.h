#ifndef HOMEWORK1_TESTMEMORY_H
#define HOMEWORK1_TESTMEMORY_H

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <algorithm>
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "WeakPtr.h"

struct MemObject {
    int value;
    double data[4];
    MemObject(int v) : value(v) {
        for (int i = 0; i < 4; ++i) data[i] = v * 1.5;
    }
};

inline void drawMemBar(const std::string& label, size_t bytes, size_t maxBytes) {
    int width = 40;
    int bars = maxBytes > 0 ? static_cast<int>(static_cast<double>(bytes) / maxBytes * width) : 0;
    std::cout << std::setw(22) << label << " | ";
    for (int i = 0; i < bars; ++i) std::cout << "#";
    for (int i = bars; i < width; ++i) std::cout << " ";
    std::cout << " | " << bytes << " bytes\n";
}

inline void runMemoryTests() {
    std::cout << "\n=====================================\n";
    std::cout << "    Тесты памяти\n";
    std::cout << "=====================================\n";

    std::cout << "\n--- Размеры самих указателей ---\n";
    std::cout << std::setw(26) << "raw T*" << ": " << sizeof(MemObject*) << " bytes\n";
    std::cout << std::setw(26) << "UnqPtr<T>" << ": " << sizeof(UnqPtr<MemObject>) << " bytes\n";
    std::cout << std::setw(26) << "ShrdPtr<T>" << ": " << sizeof(ShrdPtr<MemObject>) << " bytes\n";
    std::cout << std::setw(26) << "WeakPtr<T>" << ": " << sizeof(WeakPtr<MemObject>) << " bytes\n";
    std::cout << std::setw(26) << "std::unique_ptr<T>" << ": " << sizeof(std::unique_ptr<MemObject>) << " bytes\n";
    std::cout << std::setw(26) << "std::shared_ptr<T>" << ": " << sizeof(std::shared_ptr<MemObject>) << " bytes\n";
    std::cout << std::setw(26) << "std::weak_ptr<T>" << ": " << sizeof(std::weak_ptr<MemObject>) << " bytes\n";

    std::cout << "\n--- Размер объекта MemObject ---\n";
    std::cout << "sizeof(MemObject) = " << sizeof(MemObject) << " bytes\n";
    std::cout << "sizeof(ControlBlock для ShrdPtr/WeakPtr) = "
              << (sizeof(MemObject*) + 2 * sizeof(size_t)) << " bytes (ptr + strong + weak)\n";

    const int N = 1000000;

    std::cout << "\n--- Оверхед на " << N << " указателей ---\n";

    size_t rawSize = N * sizeof(MemObject*);
    size_t unqSize = N * sizeof(UnqPtr<MemObject>);
    size_t shrdSize = N * sizeof(ShrdPtr<MemObject>);
    size_t weakSize = N * sizeof(WeakPtr<MemObject>);
    size_t stdUnqSize = N * sizeof(std::unique_ptr<MemObject>);
    size_t stdShrdSize = N * sizeof(std::shared_ptr<MemObject>);
    size_t stdWeakSize = N * sizeof(std::weak_ptr<MemObject>);

    std::cout << std::setw(26) << "raw T*" << ": " << rawSize << " bytes\n";
    std::cout << std::setw(26) << "UnqPtr<T>" << ": " << unqSize << " bytes\n";
    std::cout << std::setw(26) << "ShrdPtr<T>" << ": " << shrdSize << " bytes\n";
    std::cout << std::setw(26) << "WeakPtr<T>" << ": " << weakSize << " bytes\n";
    std::cout << std::setw(26) << "std::unique_ptr<T>" << ": " << stdUnqSize << " bytes\n";
    std::cout << std::setw(26) << "std::shared_ptr<T>" << ": " << stdShrdSize << " bytes\n";
    std::cout << std::setw(26) << "std::weak_ptr<T>" << ": " << stdWeakSize << " bytes\n";

    std::cout << "\n--- График: оверхед на " << N << " указателей ---\n";
    size_t maxSize = std::max({rawSize, unqSize, shrdSize, weakSize, stdUnqSize, stdShrdSize, stdWeakSize});
    drawMemBar("raw T*", rawSize, maxSize);
    drawMemBar("UnqPtr<T>", unqSize, maxSize);
    drawMemBar("ShrdPtr<T>", shrdSize, maxSize);
    drawMemBar("WeakPtr<T>", weakSize, maxSize);
    drawMemBar("std::unique_ptr<T>", stdUnqSize, maxSize);
    drawMemBar("std::shared_ptr<T>", stdShrdSize, maxSize);
    drawMemBar("std::weak_ptr<T>", stdWeakSize, maxSize);

    std::cout << "\n--- Дополнительные управляющие блоки ---\n";
    size_t shrdBlock = N * (sizeof(MemObject*) + 2 * sizeof(size_t));
    size_t stdShrdBlock = N * (2 * sizeof(size_t) + sizeof(void*));
    std::cout << "ShrdPtr/WeakPtr: " << N << " блоков по "
              << (sizeof(MemObject*) + 2 * sizeof(size_t))
              << " bytes = " << shrdBlock << " bytes\n";
    std::cout << "std::shared_ptr: " << N << " блоков по "
              << (2 * sizeof(size_t) + sizeof(void*))
              << " bytes = " << stdShrdBlock << " bytes\n";

    std::cout << "\n--- График: управляющие блоки ---\n";
    size_t maxBlock = std::max(shrdBlock, stdShrdBlock);
    drawMemBar("ShrdPtr/Weak ctrl", shrdBlock, maxBlock);
    drawMemBar("std::shared ctrl", stdShrdBlock, maxBlock);

    std::cout << "\nТесты памяти завершены.\n";
}

#endif