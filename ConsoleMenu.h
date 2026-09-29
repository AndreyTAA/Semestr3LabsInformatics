#ifndef HOMEWORK1_CONSOLEMENU_H
#define HOMEWORK1_CONSOLEMENU_H

#include <iostream>
#include <string>
#include <limits>
#include "TestRunner.h"
#include "TestUnqPtr.h"
#include "TestShrdPtr.h"
#include "TestMsPtr.h"
#include "TestDynamicArray.h"
#include "TestPerformance.h"
#include "TestMemory.h"
#include "TestWeakPtr.h"

inline void resetTestCounters() {
    g_passed = 0;
    g_total = 0;
    g_nextNumber = 1;
    g_currentTestPassed = true;
}

inline void runAllFunctionalTests() {
    resetTestCounters();
    std::cout << "\n=====================================\n";
    std::cout << "    Функциональные тесты\n";
    std::cout << "=====================================\n";

    runAllUnqPtrTests();
    runAllShrdPtrTests();
    runAllWeakPtrTests();
    runAllMsPtrTests();
    runAllDynamicArrayTests();

    std::cout << "\n";
    printSummary();
}

inline void printMainMenu() {
    std::cout << "\n=====================================\n";
    std::cout << "    Лабораторная работа №1\n";
    std::cout << "    Умные указатели\n";
    std::cout << "=====================================\n";
    std::cout << "1. Запустить общие тесты (функциональные)\n";
    std::cout << "2. Тесты производительности\n";
    std::cout << "3. Тесты памяти\n";
    std::cout << "0. Выход\n";
    std::cout << "Выбор: ";
}

inline int readChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return choice;
}

inline void runConsoleMenu() {
    while (true) {
        printMainMenu();
        int choice = readChoice();
        switch (choice) {
            case 1:
                runAllFunctionalTests();
                break;
            case 2:
                runPerformanceTests();
                break;
            case 3:
                runMemoryTests();
                break;
            case 0:
                std::cout << "\nВыход из программы.\n";
                return;
            default:
                std::cout << "\nНеверный выбор, попробуйте снова.\n";
                break;
        }
    }
}

#endif