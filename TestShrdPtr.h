#ifndef HOMEWORK1_TESTSHRDPTR_H
#define HOMEWORK1_TESTSHRDPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "ShrdPtr.h"
#include "TestRunner.h"

struct ShrdTracked {
    static int alive;
    int value;
    ShrdTracked(int v) : value(v) { ++alive; }
    ~ShrdTracked() { --alive; }
};
int ShrdTracked::alive = 0;

struct ShrdBase {
    virtual ~ShrdBase() = default;
    int base = 10;
};
struct ShrdDerived : ShrdBase {
    int derived = 20;
};

inline void testShrdPtrDefault() {
    beginTest();
    try {
        ShrdPtr<int> p;
        check(p.get() == nullptr, "get() == nullptr у пустого ShrdPtr");
        check(!p, "operator bool == false у пустого ShrdPtr");
        check(p.use_count() == 0, "use_count() == 0 у пустого ShrdPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём ShrdPtr по умолчанию. Проверяем, что он пуст: get() "
        "возвращает nullptr, operator bool даёт false, use_count() равен 0 "
        "(нет ни одного сильного владельца). Это корректное «нулевое» состояние."
    );
}

inline void testShrdPtrFromRaw() {
    beginTest();
    try {
        auto p = makeShrd<int>(42);
        check(p.get() != nullptr, "get() != nullptr после makeShrd");
        check(*p == 42, "*p == 42 (значение из makeShrd)");
        check(p.use_count() == 1, "use_count() == 1 (единственный владелец)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём ShrdPtr через фабрику makeShrd<int>(42). Создаётся "
        "ControlBlock со strong_ = 1 и weak_ = 1 (неявная weak от самого "
        "ShrdPtr). Проверяем: указатель не пуст, значение 42, use_count == 1."
    );
}

inline void testShrdPtrFromNullptr() {
    beginTest();
    try {
        ShrdPtr<int> p(nullptr);
        check(p.get() == nullptr, "get() == nullptr после конструктора из nullptr");
        check(p.use_count() == 0, "use_count() == 0 у nullptr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём ShrdPtr из nullptr. Указатель пуст, use_count равен 0. "
        "Не должно быть попытки создать ControlBlock — иначе это была бы "
        "бессмысленная аллокация."
    );
}

inline void testShrdPtrCopyCtor() {
    beginTest();
    try {
        auto a = makeShrd<int>(5);
        ShrdPtr<int> b(a);
        check(a.use_count() == 2, "use_count у a == 2 после копирования");
        check(b.use_count() == 2, "use_count у b == 2 после копирования");
        check(*a == *b, "значения a и b совпадают");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем копирующий конструктор. a владеет объектом, b(a) — копия. "
        "Оба смотрят на один ControlBlock, use_count инкрементируется до 2."
    );
}

inline void testShrdPtrCopyAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(1);
        auto b = makeShrd<int>(2);
        b = a;
        check(a.use_count() == 2, "use_count у a == 2 после присваивания");
        check(b.use_count() == 2, "use_count у b == 2 после присваивания");
        check(*b == 1, "b теперь смотрит на значение 1 (старое значение 2 удалено)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем копирующее присваивание. До операции a владел 1, b владел 2. "
        "После b = a: старый объект b (значение 2) удаляется, b становится "
        "копией a. use_count у a и b == 2, значение у b — 1. Реализовано "
        "через copy-and-swap."
    );
}

inline void testShrdPtrMoveCtor() {
    beginTest();
    try {
        auto a = makeShrd<int>(3);
        ShrdPtr<int> b(std::move(a));
        check(a.get() == nullptr, "источник пуст после перемещения");
        check(a.use_count() == 0, "use_count источника == 0");
        check(b.use_count() == 1, "use_count приёмника == 1");
        check(*b == 3, "значение 3 переехало вместе с владением");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающий конструктор. Перемещение отличается от "
        "копирования тем, что счётчик use_count НЕ увеличивается — владелец "
        "просто переезжает. После std::move(a): у a get() == nullptr и "
        "use_count == 0, у b use_count == 1."
    );
}

inline void testShrdPtrMoveAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(4);
        auto b = makeShrd<int>(5);
        b = std::move(a);
        check(a.get() == nullptr, "источник пуст после перемещающего присваивания");
        check(*b == 4, "b получил значение 4 (старое значение 5 удалено)");
        check(b.use_count() == 1, "use_count у b == 1 после перемещения");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающее присваивание. b = std::move(a) удаляет "
        "старый объект b, забирает блок от a. use_count не увеличивается, "
        "владелец переезжает. Проверяем: a пуст, b владеет значением 4."
    );
}

inline void testShrdPtrSelfAssign() {
    beginTest();
    try {
        auto a = makeShrd<int>(7);
        ShrdPtr<int>& ref = a;
        a = ref;
        check(a.use_count() == 1, "use_count не изменился после самоприсваивания");
        check(*a == 7, "значение 7 не потеряно");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Самоприсваивание a = a (через ссылку). Наивная реализация сначала "
        "уменьшила бы счётчик, потом увеличила, но если счётчик был 1 — "
        "она удалила бы объект и оставила висячий указатель. Проверяем, "
        "что защита if (this != &other) работает."
    );
}

inline void testShrdPtrUseCountDecrease() {
    beginTest();
    try {
        auto a = makeShrd<int>(5);
        {
            ShrdPtr<int> b(a);
            ShrdPtr<int> c(a);
            check(a.use_count() == 3, "use_count == 3 при трёх владельцах");
        }
        check(a.use_count() == 1, "use_count == 1 после выхода копий из области видимости");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем уменьшение use_count при удалении копий. Создаём a, "
        "потом b и c как копии. use_count == 3. Выходим из внутренней "
        "области видимости — b и c удаляются, use_count возвращается к 1. "
        "Сам объект при этом жив — потому что a всё ещё держит его."
    );
}

inline void testShrdPtrDestructorDeletes() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b(a);
            check(ShrdTracked::alive == 1, "alive == 1 пока есть владельцы");
            check(a.use_count() == 2, "use_count == 2 при двух владельцах");
        }
        check(ShrdTracked::alive == 0, "alive == 0 после удаления всех владельцев");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем, что объект удаляется только когда уходит последний "
        "владелец. Создаём a и b (копию). alive == 1, use_count == 2. "
        "Выходим из области видимости — оба ShrdPtr разрушаются. "
        "После первого use_count == 1, объект жив. После второго "
        "use_count == 0 — объект удаляется, alive == 0."
    );
}

inline void testShrdPtrReset() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        auto a = makeShrd<ShrdTracked>(1);
        ShrdPtr<ShrdTracked> b(a);
        a.reset(new ShrdTracked(2));
        check(ShrdTracked::alive == 2, "alive == 2 (старый объект жив из-за b, новый создан)");
        check(b.use_count() == 1, "use_count у b == 1 (b — единственный владелец старого)");
        a.reset();
        check(ShrdTracked::alive == 1, "alive == 1 после a.reset() (удалён объект 2)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем reset() с разными аргументами. Изначально a и b держат "
        "объект 1 (alive == 1). a.reset(new ShrdTracked(2)) не удаляет "
        "объект 1, потому что b ещё держит его. Затем a.reset() удаляет "
        "объект 2, alive == 1."
    );
}

inline void testShrdPtrNoLeakAllReleased() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b = a;
            ShrdPtr<ShrdTracked> c = a;
            check(a.use_count() == 3, "use_count == 3 при трёх владельцах");
        }
        check(ShrdTracked::alive == 0, "alive == 0 — утечек нет");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем отсутствие утечек при массовом копировании. Создаём "
        "три ShrdPtr на один объект. При выходе из области все три "
        "разрушаются по очереди: use_count уменьшается 3 → 2 → 1 → 0. "
        "Когда достигает 0 — объект удаляется. alive == 0 — утечки нет."
    );
}

inline void testShrdPtrSubtyping() {
    beginTest();
    try {
        auto d = makeShrd<ShrdDerived>();
        ShrdPtr<ShrdBase> b(d);
        check(d.use_count() == 2, "use_count у d == 2 после создания b");
        check(b.use_count() == 2, "use_count у b == 2 (общий счётчик)");
        check(b->base == 10, "доступ к base через ShrdPtr<Base>");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем подтипизацию: ShrdPtr<Derived> → ShrdPtr<Base>. "
        "Возможно, потому что ControlBlock нешаблонный — ShrdPtr<Base> и "
        "ShrdPtr<Derived> хранят один и тот же ControlBlock*. use_count "
        "у обоих == 2 (общий счётчик)."
    );
}

inline void testShrdPtrMakeShrd() {
    beginTest();
    try {
        auto p = makeShrd<int>(123);
        check(*p == 123, "makeShrd<int>(123) создаёт объект со значением 123");
        check(p.use_count() == 1, "use_count == 1 после makeShrd");
        auto q = makeShrd<std::string>("world");
        check(*q == "world", "makeShrd<std::string> работает для не-POD");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем фабрику makeShrd. Это единственный публичный способ "
        "создать ShrdPtr из сырого объекта — конструктор от T* приватный. "
        "Фабрика делает new T(...) и заворачивает в ShrdPtr."
    );
}

inline void testShrdPtrManyOwners() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        const int N = 100;
        {
            auto p = makeShrd<ShrdTracked>(0);
            ShrdPtr<ShrdTracked>* owners = new ShrdPtr<ShrdTracked>[N];
            for (int i = 0; i < N; ++i) {
                owners[i] = p;
            }
            check(p.use_count() == N + 1, "use_count == N+1 (все копии + сам p)");
            delete[] owners;
            check(p.use_count() == 1, "use_count == 1 после delete[] owners");
            check(ShrdTracked::alive == 1, "alive == 1 (объект жив, потому что p ещё держит)");
        }
        check(ShrdTracked::alive == 0, "alive == 0 после выхода p из области видимости");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем работу с большим количеством владельцев. Создаём массив "
        "из 100 ShrdPtr, каждый — копия p. use_count == 101. delete[] owners "
        "удаляет все копии, use_count возвращается к 1. При выходе p из "
        "блока объект удаляется."
    );
}

inline void testShrdPtrNoLeakAfterMove() {
    beginTest();
    try {
        ShrdTracked::alive = 0;
        {
            auto a = makeShrd<ShrdTracked>(1);
            ShrdPtr<ShrdTracked> b(std::move(a));
            check(a.get() == nullptr, "источник пуст после перемещения");
            check(ShrdTracked::alive == 1, "объект жив после перемещения (alive == 1)");
        }
        check(ShrdTracked::alive == 0, "alive == 0 после удаления последнего владельца");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем отсутствие утечек при перемещении. a перемещается в b. "
        "Объект остаётся один — не создаётся и не удаляется лишнего. "
        "При выходе из блока b разрушается, объект удаляется. alive == 0."
    );
}

inline void runAllShrdPtrTests() {
    std::cout << "\n===== Тесты ShrdPtr =====\n\n";
    testShrdPtrDefault();
    testShrdPtrFromRaw();
    testShrdPtrFromNullptr();
    testShrdPtrCopyCtor();
    testShrdPtrCopyAssign();
    testShrdPtrMoveCtor();
    testShrdPtrMoveAssign();
    testShrdPtrSelfAssign();
    testShrdPtrUseCountDecrease();
    testShrdPtrDestructorDeletes();
    testShrdPtrReset();
    testShrdPtrNoLeakAllReleased();
    testShrdPtrSubtyping();
    testShrdPtrMakeShrd();
    testShrdPtrManyOwners();
    testShrdPtrNoLeakAfterMove();
}

#endif