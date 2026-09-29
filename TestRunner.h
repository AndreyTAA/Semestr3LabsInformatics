#ifndef HOMEWORK1_TESTRUNNER_H
#define HOMEWORK1_TESTRUNNER_H

#include <iostream>
#include <string>

inline int g_passed = 0;
inline int g_total = 0;
inline int g_nextNumber = 1;
inline bool g_currentTestPassed = true;

inline void beginTest() {
    g_currentTestPassed = true;
}

inline void check(bool condition) {
    if (!condition) {
        g_currentTestPassed = false;
    }
}

inline void reportResult(const std::string& description) {
    ++g_total;
    if (g_currentTestPassed) {
        ++g_passed;
    }
    std::cout << "Тест №" << g_nextNumber++ << "\n";
    std::cout << "Описание: " << description << "\n";
    std::cout << "Вердикт[PASS/FAILED]: " << (g_currentTestPassed ? "[pass]" : "[failed]") << "\n\n";
}

inline void printSummary() {
    std::cout << "=====================================\n";
    std::cout << "Тестов пройдено: " << g_passed << "/" << g_total << "\n";
    std::cout << "=====================================\n";
}

#endif