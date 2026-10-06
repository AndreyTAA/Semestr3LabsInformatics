#ifndef HOMEWORK1_TESTRUNNER_H
#define HOMEWORK1_TESTRUNNER_H

#include <iostream>
#include <string>
#include <vector>

inline int g_passed = 0;
inline int g_total = 0;
inline int g_nextNumber = 1;
inline bool g_currentTestPassed = true;
inline std::vector<std::string> g_failedChecks;

inline void beginTest() {
    g_currentTestPassed = true;
    g_failedChecks.clear();
}

inline void check(bool condition, const std::string& description) {
    if (!condition) {
        g_currentTestPassed = false;
        g_failedChecks.push_back(description);
    }
}

inline void reportResult(const std::string& description) {
    ++g_total;
    if (g_currentTestPassed) {
        ++g_passed;
    }
    std::cout << "Тест №" << g_nextNumber++ << "\n";
    std::cout << "Описание: " << description << "\n";
    std::cout << "Вердикт: " << (g_currentTestPassed ? "[pass]" : "[failed]") << "\n";
    if (!g_currentTestPassed) {
        std::cout << "Проваленные проверки:\n";
        for (const auto& s : g_failedChecks) {
            std::cout << "  - " << s << "\n";
        }
    }
    std::cout << "\n";
}

inline void printSummary() {
    std::cout << "=====================================\n";
    std::cout << "Тестов пройдено: " << g_passed << "/" << g_total << "\n";
    std::cout << "=====================================\n";
}

#endif