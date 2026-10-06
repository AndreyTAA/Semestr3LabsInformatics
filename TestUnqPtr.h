#ifndef HOMEWORK1_TESTUNQPTR_H
#define HOMEWORK1_TESTUNQPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "UnqPtr.h"
#include "TestRunner.h"

struct UnqTracked {
    static int alive;
    int value;
    UnqTracked(int v) : value(v) { ++alive; }
    ~UnqTracked() { --alive; }
    UnqTracked(const UnqTracked&) = delete;
    UnqTracked& operator=(const UnqTracked&) = delete;
};
int UnqTracked::alive = 0;

struct UnqBase {
    virtual ~UnqBase() = default;
    int base = 1;
};
struct UnqDerived : UnqBase {
    int derived = 2;
};

inline void testUnqPtrDefault() {
    beginTest();
    try {
        UnqPtr<int> p;
        check(p.get() == nullptr, "get() возвращает nullptr у пустого UnqPtr");
        check(!p, "operator bool возвращает false у пустого UnqPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём UnqPtr по умолчанию (без аргументов). Проверяем, что он "
        "пустой: get() возвращает nullptr, operator bool даёт false. "
        "Это базовое состояние перед получением владения каким-либо объектом."
    );
}

inline void testUnqPtrFromRaw() {
    beginTest();
    try {
        auto p = makeUnq<int>(42);
        check(p.get() != nullptr, "get() не nullptr после makeUnq");
        check(*p == 42, "*p == 42 (значение, переданное в makeUnq)");
        check(static_cast<bool>(p), "operator bool == true для владеющего указателя");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём UnqPtr через фабрику makeUnq<int>(42). Фабрика делает "
        "new int(42), заворачивает его в UnqPtr. Проверяем, что указатель "
        "не пустой, разыменование даёт 42, и operator bool возвращает true."
    );
}

inline void testUnqPtrFromNullptr() {
    beginTest();
    try {
        UnqPtr<int> p(nullptr);
        check(p.get() == nullptr, "get() == nullptr после конструктора из nullptr");
        check(!p, "operator bool == false при nullptr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём UnqPtr из nullptr. Проверяем, что указатель остаётся пустым, "
        "и что деструктор не пытается удалить nullptr — это безопасно, "
        "но важно, чтобы не было разыменования."
    );
}

inline void testUnqPtrMoveCtor() {
    beginTest();
    try {
        auto a = makeUnq<int>(10);
        UnqPtr<int> b(std::move(a));
        check(a.get() == nullptr, "источник пуст после перемещения (a.get() == nullptr)");
        check(b.get() != nullptr, "приёмник получил владение (b.get() != nullptr)");
        check(*b == 10, "значение объекта сохранилось (*b == 10)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающий конструктор. Создаём a = makeUnq<int>(10), "
        "перемещаем его в b через std::move. После перемещения у a должен "
        "быть get() == nullptr (владение ушло), у b — тот же объект со "
        "значением 10. Семантика перемещения: владелец один, но переезжает."
    );
}

inline void testUnqPtrMoveAssign() {
    beginTest();
    try {
        auto a = makeUnq<int>(1);
        auto b = makeUnq<int>(2);
        b = std::move(a);
        check(a.get() == nullptr, "источник пуст после перемещающего присваивания");
        check(*b == 1, "b теперь владеет объектом со значением 1 (старый объект удалён)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающее присваивание. Изначально b владеет объектом "
        "со значением 2, a — объектом со значением 1. При b = std::move(a) "
        "старый объект b (значение 2) удаляется, b забирает объект a (значение 1), "
        "у a остаётся nullptr."
    );
}

inline void testUnqPtrMoveAssignToSelf() {
    beginTest();
    try {
        auto a = makeUnq<int>(5);
        int* raw = a.get();
        a = std::move(a);
        check(a.get() == raw, "самоперемещение не должно менять указатель");
        check(*a == 5, "значение 5 сохранилось после самоперемещения");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Самоперемещение: a = std::move(a). Наивная реализация сначала "
        "удалила бы старый объект, а потом попыталась бы взять у себя же "
        "nullptr. Проверяем защиту if (this != &other)."
    );
}

inline void testUnqPtrRelease() {
    beginTest();
    try {
        auto p = makeUnq<int>(7);
        int* raw = p.release();
        check(p.get() == nullptr, "p.get() == nullptr после release");
        check(raw != nullptr, "release вернул ненулевой указатель");
        check(*raw == 7, "*raw == 7 (значение сохранено в отданном указателе)");
        delete raw;
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем release(). Он отпускает владение, возвращая сырой указатель "
        "наружу и обнуляя UnqPtr. Полученный raw нужно удалить вручную."
    );
}

inline void testUnqPtrReset() {
    beginTest();
    try {
        auto p = makeUnq<int>(1);
        int* old = p.get();
        p.reset(new int(2));
        check(p.get() != old, "reset(new) заменил указатель");
        check(*p == 2, "*p == 2 (значение нового объекта)");
        p.reset();
        check(p.get() == nullptr, "reset() без аргумента обнулил указатель");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем reset(). Сначала объект 1, потом p.reset(new int(2)) "
        "удаляет старый и берёт новый. Затем p.reset() удаляет и обнуляет."
    );
}

inline void testUnqPtrResetSame() {
    beginTest();
    try {
        auto p = makeUnq<int>(3);
        int* raw = p.get();
        p.reset(raw);
        check(p.get() == raw, "reset тем же указателем не должен менять get()");
        check(*p == 3, "значение 3 не потеряно (объект не удалён)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Особый случай reset: если передать тот же указатель, объект НЕ должен "
        "удаляться, иначе получим разыменование освобождённой памяти. "
        "Проверяем, что защита if (p != ptr_) работает."
    );
}

inline void testUnqPtrSwap() {
    beginTest();
    try {
        auto a = makeUnq<int>(1);
        auto b = makeUnq<int>(2);
        a.swap(b);
        check(*a == 2, "a получил значение 2 после swap");
        check(*b == 1, "b получил значение 1 после swap");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем swap(). Обмен содержимым двух UnqPtr. После a.swap(b) "
        "a владеет объектом 2, b — объектом 1. Обмен O(1), без копирования."
    );
}

inline void testUnqPtrDestructorDeletes() {
    beginTest();
    try {
        UnqTracked::alive = 0;
        {
            auto p = makeUnq<UnqTracked>(5);
            check(UnqTracked::alive == 1, "alive == 1 после создания UnqTracked");
        }
        check(UnqTracked::alive == 0, "alive == 0 после разрушения UnqPtr (объект удалён)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем автоматическое удаление объекта. UnqTracked считает "
        "живые экземпляры через статический счётчик. При выходе UnqPtr "
        "из области видимости delete ptr_ удаляет объект."
    );
}

inline void testUnqPtrDerived() {
    beginTest();
    try {
        UnqPtr<UnqBase> p = makeUnq<UnqDerived>();
        check(p->base == 1, "доступ к полю base базового класса через UnqPtr<Base>");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Хранение производного объекта через указатель на базовый. "
        "Создаём UnqDerived, держим как UnqPtr<UnqBase>. Доступ к base "
        "работает через up-cast. Деструктор вызовется правильный."
    );
}

inline void testUnqPtrSubtyping() {
    beginTest();
    try {
        auto d = makeUnq<UnqDerived>();
        UnqPtr<UnqBase> b(std::move(d));
        check(d.get() == nullptr, "источник пуст после подтипизации-move");
        check(b.get() != nullptr, "приёмник получил владение");
        check(b->base == 1, "доступ к base через UnqPtr<Base>");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Подтипизация при перемещении: UnqPtr<Derived> → UnqPtr<Base>. "
        "Возможно только через move — потому что UnqPtr не копируется. "
        "Шаблонный конструктор с enable_if_t<is_base_of_v> разрешает "
        "преобразование, только если Derived действительно наследник Base."
    );
}

inline void testUnqPtrMakeUnq() {
    beginTest();
    try {
        auto p = makeUnq<int>(99);
        check(*p == 99, "makeUnq<int>(99) создаёт объект со значением 99");
        auto q = makeUnq<std::string>("hello");
        check(*q == "hello", "makeUnq<std::string>(\"hello\") работает для не-POD типа");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем фабрику makeUnq. Это рекомендуемый способ создания "
        "UnqPtr — конструктор от T* приватный. Фабрика делает new T(...) "
        "с perfect forwarding аргументов."
    );
}

inline void testUnqPtrMultipleAllocs() {
    beginTest();
    try {
        const int N = 1000;
        UnqTracked::alive = 0;
        {
            UnqPtr<UnqTracked>* arr = new UnqPtr<UnqTracked>[N];
            for (int i = 0; i < N; ++i) {
                arr[i] = makeUnq<UnqTracked>(i);
            }
            check(UnqTracked::alive == N, "alive == N после создания N объектов");
            delete[] arr;
        }
        check(UnqTracked::alive == 0, "alive == 0 после delete[] — утечек нет");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем работу с массивом UnqPtr. Создаём N = 1000 UnqPtr, "
        "каждому свой UnqTracked. delete[] arr вызывает деструкторы "
        "каждого UnqPtr. Проверка на отсутствие утечек."
    );
}

inline void testUnqPtrNoLeakOnException() {
    beginTest();
    try {
        UnqTracked::alive = 0;
        try {
            auto p = makeUnq<UnqTracked>(1);
            throw std::runtime_error("test");
        } catch (...) {}
        check(UnqTracked::alive == 0, "alive == 0 после исключения — RAII сработал");
    } catch (...) {
        check(false, "тест упал с исключением вне try");
    }
    reportResult(
        "Устойчивость к исключениям. Создаём UnqTracked, выбрасываем "
        "исключение. При раскрутке стека деструктор UnqPtr срабатывает "
        "и удаляет объект — базовая гарантия RAII."
    );
}

inline void runAllUnqPtrTests() {
    std::cout << "\n===== Тесты UnqPtr =====\n\n";
    testUnqPtrDefault();
    testUnqPtrFromRaw();
    testUnqPtrFromNullptr();
    testUnqPtrMoveCtor();
    testUnqPtrMoveAssign();
    testUnqPtrMoveAssignToSelf();
    testUnqPtrRelease();
    testUnqPtrReset();
    testUnqPtrResetSame();
    testUnqPtrSwap();
    testUnqPtrDestructorDeletes();
    testUnqPtrDerived();
    testUnqPtrSubtyping();
    testUnqPtrMakeUnq();
    testUnqPtrMultipleAllocs();
    testUnqPtrNoLeakOnException();
}

#endif