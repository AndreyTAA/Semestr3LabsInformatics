#ifndef HOMEWORK1_TESTWEAKPTR_H
#define HOMEWORK1_TESTWEAKPTR_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "ShrdPtr.h"
#include "WeakPtr.h"
#include "TestRunner.h"

struct WeakTracked {
    static int alive;
    int value;
    WeakTracked(int v) : value(v) { ++alive; }
    ~WeakTracked() { --alive; }
};
int WeakTracked::alive = 0;

inline void testWeakPtrDefault() {
    beginTest();
    try {
        WeakPtr<int> w;
        check(w.expired(), "expired() == true у пустого WeakPtr");
        check(w.use_count() == 0, "use_count() == 0 у пустого WeakPtr");
        check(w.lock().get() == nullptr, "lock() возвращает пустой ShrdPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём WeakPtr по умолчанию. У него нет блока — он пустой. "
        "expired() возвращает true (объекта нет), use_count == 0, "
        "lock() возвращает пустой ShrdPtr. Корректное «ни на что не смотрю»."
    );
}

inline void testWeakPtrFromShrd() {
    beginTest();
    try {
        auto s = makeShrd<int>(42);
        WeakPtr<int> w(s);
        check(!w.expired(), "expired() == false (объект жив через s)");
        check(w.use_count() == 1, "use_count() == 1 (только s — сильный владелец)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Создаём WeakPtr из существующего ShrdPtr. WeakPtr берёт тот же "
        "ControlBlock, но НЕ увеличивает strong_ — только weak_. "
        "Проверяем: expired() == false, use_count == 1."
    );
}

inline void testWeakPtrLockAlive() {
    beginTest();
    try {
        auto s = makeShrd<int>(42);
        WeakPtr<int> w(s);
        ShrdPtr<int> locked = w.lock();
        check(locked.get() != nullptr, "lock() вернул ненулевой ShrdPtr");
        check(*locked == 42, "*locked == 42 (значение объекта)");
        check(s.use_count() == 2, "use_count == 2 (s + locked)");
        check(w.use_count() == 2, "use_count у w тоже == 2 (общий счётчик)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем lock() на живом объекте. lock() временно превращает "
        "слабую ссылку в сильную — создаёт новый ShrdPtr, увеличивая "
        "strong_. Теперь use_count у s == 2. locked гарантированно не "
        "будет удалён, пока жив."
    );
}

inline void testWeakPtrLockExpired() {
    beginTest();
    try {
        WeakPtr<int> w;
        {
            auto s = makeShrd<int>(42);
            w = s;
            check(!w.expired(), "expired() == false, пока s жив");
        }
        check(w.expired(), "expired() == true после смерти объекта");
        ShrdPtr<int> locked = w.lock();
        check(locked.get() == nullptr, "lock() на мёртвом объекте возвращает nullptr");
        check(w.use_count() == 0, "use_count == 0 после смерти объекта");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем lock() на мёртвом объекте. Создаём s внутри блока и "
        "связываем w с ним. При выходе s разрушается, strong_ = 0, объект "
        "удаляется. WeakPtr остаётся жив (weak_ > 0), expired() == true, "
        "lock() возвращает пустой ShrdPtr."
    );
}

inline void testWeakPtrCopyCtor() {
    beginTest();
    try {
        auto s = makeShrd<int>(5);
        WeakPtr<int> w1(s);
        WeakPtr<int> w2(w1);
        check(!w1.expired(), "w1 не expired после копирования");
        check(!w2.expired(), "w2 не expired после копирования");
        check(w1.use_count() == 1, "use_count == 1 (только s владеет)");
        check(w2.use_count() == 1, "use_count у w2 тоже == 1");
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(s.use_count() == 3, "use_count == 3 после двух lock (s + l1 + l2)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем копирующий конструктор WeakPtr. w2(w1) увеличивает "
        "weak_, но не strong_. Сильных владельцев по-прежнему 1 (только s). "
        "После двух lock() strong_ становится 3."
    );
}

inline void testWeakPtrCopyAssign() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w2 = w1;
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(l1.get() == l2.get(), "lock() обоих WeakPtr даёт один и тот же указатель");
        check(*l1 == 1, "*l1 == 1 (значение s1)");
        check(*l2 == 1, "*l2 == 1 (w2 переключился на s1)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем копирующее присваивание WeakPtr. w2 отвязывается от s2 "
        "(weak_ блока s2 уменьшается) и привязывается к блоку s1. "
        "После этого оба WeakPtr смотрят на один объект."
    );
}

inline void testWeakPtrMoveCtor() {
    beginTest();
    try {
        auto s = makeShrd<int>(7);
        WeakPtr<int> w1(s);
        WeakPtr<int> w2(std::move(w1));
        check(!w2.expired(), "w2 не expired после move");
        check(w1.expired(), "w1 expired после move (block_ == nullptr)");
        auto l = w2.lock();
        check(l.get() != nullptr, "lock() через w2 работает");
        check(*l == 7, "значение 7 доступно через w2");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающий конструктор WeakPtr. После move у w1 "
        "block_ == nullptr — он становится expired. w2 получает блок "
        "и работает нормально. Счётчики strong_ и weak_ не меняются."
    );
}

inline void testWeakPtrMoveAssign() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w2 = std::move(w1);
        check(!w2.expired(), "w2 не expired после move-присваивания");
        check(w1.expired(), "w1 expired после move-присваивания");
        auto l = w2.lock();
        check(*l == 1, "w2 теперь смотрит на значение 1");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем перемещающее присваивание WeakPtr. w2 отпускает свой "
        "старый блок (decWeak), забирает блок от w1, w1 становится пустым. "
        "Используется явный decWeak вместо swap — иначе старая ссылка утекла бы."
    );
}

inline void testWeakPtrAssignFromShrd() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w(s1);
        w = s2;
        auto l = w.lock();
        check(l.get() != nullptr, "lock() работает после присваивания от ShrdPtr");
        check(*l == 2, "w теперь смотрит на s2 (значение 2)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем присваивание WeakPtr от ShrdPtr. w сначала слабо смотрит "
        "на s1, потом w = s2 переключает его на s2. Проверяем, что lock() "
        "возвращает указатель на объект s2 (значение 2)."
    );
}

inline void testWeakPtrReset() {
    beginTest();
    try {
        auto s = makeShrd<int>(1);
        WeakPtr<int> w(s);
        check(!w.expired(), "w не expired до reset");
        w.reset();
        check(w.expired(), "w expired после reset");
        check(w.use_count() == 0, "use_count == 0 после reset");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем reset(). WeakPtr отвязывается от объекта, вызывая "
        "decWeak на блоке. После reset w.expired() == true, use_count == 0. "
        "Объект при этом жив, потому что s его всё ещё держит."
    );
}

inline void testWeakPtrSwap() {
    beginTest();
    try {
        auto s1 = makeShrd<int>(1);
        auto s2 = makeShrd<int>(2);
        WeakPtr<int> w1(s1);
        WeakPtr<int> w2(s2);
        w1.swap(w2);
        auto l1 = w1.lock();
        auto l2 = w2.lock();
        check(*l1 == 2, "w1 после swap смотрит на значение 2 (от s2)");
        check(*l2 == 1, "w2 после swap смотрит на значение 1 (от s1)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем swap. Обмениваем содержимое двух WeakPtr: w1 смотрел "
        "на s1, w2 на s2, после swap наоборот. Проверяем через lock(), "
        "что w1 теперь видит значение 2, w2 — значение 1."
    );
}

inline void testWeakPtrDoesNotDelete() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w(s);
            check(WeakTracked::alive == 1, "alive == 1, пока s жив");
        }
        check(WeakTracked::alive == 0, "alive == 0 после смерти s (WeakPtr не удержал)");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Главная проверка: WeakPtr не удерживает объект живым. Пока s жив, "
        "alive == 1. При выходе s разрушается, объект удаляется несмотря "
        "на то, что w ещё существует. Это ключевое отличие WeakPtr от ShrdPtr."
    );
}

inline void testWeakPtrKeepsBlockAlive() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        WeakPtr<WeakTracked> w;
        {
            auto s = makeShrd<WeakTracked>(1);
            w = s;
            check(!w.expired(), "w не expired, пока s жив");
        }
        check(WeakTracked::alive == 0, "alive == 0 (объект удалён)");
        check(w.expired(), "w expired после смерти объекта");
        check(w.lock().get() == nullptr, "lock() возвращает пустой ShrdPtr");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем, что ControlBlock переживает объект. После смерти "
        "объекта (alive == 0) WeakPtr всё ещё может проверить expired() — "
        "это возможно только потому, что блок не удалён. weak_ > 0, "
        "блок жив, lock() корректно возвращает пустой ShrdPtr."
    );
}

inline void testWeakPtrLockExtendsLife() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w(s);

            ShrdPtr<WeakTracked> locked = w.lock();
            check(locked.get() != nullptr, "lock() вернул живой ShrdPtr");
            check(WeakTracked::alive == 1, "объект жив");

            s.reset();
            check(WeakTracked::alive == 1, "объект жив после s.reset(), потому что locked его держит");
            check(locked.use_count() == 1, "use_count == 1 (только locked)");
            check(w.use_count() == 1, "use_count у w тоже == 1 (общий счётчик)");
            check(!w.expired(), "w не expired, пока locked жив");
        }
        check(WeakTracked::alive == 0, "alive == 0 после выхода locked из области");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем, что lock() продлевает жизнь объекта. Создаём s и w. "
        "Затем locked = w.lock() — теперь два сильных владельца. "
        "s.reset() — остаётся только locked, но объект жив. alive == 0 "
        "только после выхода locked из области видимости."
    );
}

inline void testWeakPtrManyWeak() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        const int N = 100;
        {
            auto s = makeShrd<WeakTracked>(0);
            WeakPtr<WeakTracked>* arr = new WeakPtr<WeakTracked>[N];
            for (int i = 0; i < N; ++i) {
                arr[i] = s;
            }
            check(s.use_count() == 1, "use_count == 1 — WeakPtr не владеют");
            check(WeakTracked::alive == 1, "alive == 1 пока s жив");
            delete[] arr;
            check(s.use_count() == 1, "use_count по-прежнему 1 после delete[] arr");
            check(WeakTracked::alive == 1, "alive == 1 после delete[] arr");
        }
        check(WeakTracked::alive == 0, "alive == 0 после выхода s из области");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем, что 100 WeakPtr не влияют на сильный счётчик. "
        "Создаём массив из 100 слабых ссылок на один объект. use_count "
        "у s остаётся 1. Объект удаляется только при выходе s из блока."
    );
}

inline void testWeakPtrCycle() {
    beginTest();
    try {
        struct Node {
            ShrdPtr<Node> next;
            WeakPtr<Node> prev;
            int value;
            Node(int v) : value(v) {}
        };

        auto a = makeShrd<Node>(1);
        auto b = makeShrd<Node>(2);
        a->next = b;
        b->prev = a;

        check(a.use_count() == 1, "use_count у a == 1 (только внешняя ссылка)");
        check(b.use_count() == 2, "use_count у b == 2 (внешняя + a->next)");

        a.reset();
        check(b->prev.expired(), "b->prev.expired() == true после смерти a");
        b.reset();
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Классический сценарий использования WeakPtr — разрыв цикла. "
        "Узел a хранит СИЛЬНУЮ ссылку next на b, а b хранит СЛАБУЮ ссылку "
        "prev на a. Если бы обе были сильными — был бы цикл и утечка. "
        "После a.reset() b->prev.expired() == true."
    );
}

inline void testWeakPtrNoLeak() {
    beginTest();
    try {
        WeakTracked::alive = 0;
        {
            auto s = makeShrd<WeakTracked>(1);
            WeakPtr<WeakTracked> w1(s);
            WeakPtr<WeakTracked> w2 = w1;
            WeakPtr<WeakTracked> w3 = w2;
        }
        check(WeakTracked::alive == 0, "alive == 0 — утечек нет");
    } catch (...) {
        check(false, "тест упал с исключением");
    }
    reportResult(
        "Проверяем отсутствие утечек при множестве слабых ссылок. "
        "Создаём три WeakPtr на один объект. При выходе из блока порядок "
        "разрушения: s (объект удаляется), затем w1, w2, w3 (уменьшают weak_). "
        "Когда weak_ == 0, ControlBlock удаляется. alive == 0."
    );
}

inline void runAllWeakPtrTests() {
    std::cout << "\n===== Тесты WeakPtr =====\n\n";
    testWeakPtrDefault();
    testWeakPtrFromShrd();
    testWeakPtrLockAlive();
    testWeakPtrLockExpired();
    testWeakPtrCopyCtor();
    testWeakPtrCopyAssign();
    testWeakPtrMoveCtor();
    testWeakPtrMoveAssign();
    testWeakPtrAssignFromShrd();
    testWeakPtrReset();
    testWeakPtrSwap();
    testWeakPtrDoesNotDelete();
    testWeakPtrKeepsBlockAlive();
    testWeakPtrLockExtendsLife();
    testWeakPtrManyWeak();
    testWeakPtrCycle();
    testWeakPtrNoLeak();
}

#endif