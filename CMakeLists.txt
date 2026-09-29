cmake_minimum_required(VERSION 3.20)
project(Homework1)

set(CMAKE_CXX_STANDARD 20)


add_executable(Homework1 main.cpp
        UnqPtr.h
        ShrdPtr.h
        MemorySpan.h
        MsPtr.h
        TestUnqPtr.h
        TestRunner.h
        TestShrdPtr.h
        TestMsPtr.h
        TestDynamicArray.h
        DynamicArray.h
        TestPerformance.h
        TestMemory.h
        ConsoleMenu.h
        WeakPtr.h
        ControlBlock.h
        TestWeakPtr.h
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")