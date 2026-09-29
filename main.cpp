#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include "ConsoleMenu.h"


int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    /*int a = 5;
    int* aPtr = &a;
    UnqPtr<int> uPtr(aPtr+100); */ // конструкотор от указателя приватный

    /* void f(UnqPtr a);
     * UnqPtr b; initialized
     * f(b)
     * f(std::move(b));
     *
     *
     * void f(int, double);
     * f(5,10.0);
     *
     *const std::string&& tmp = "a"
     * rvalue нет адреса в памяти или его не знаем
     */
    // папками по указателям



    runConsoleMenu();

    return 0;



}

