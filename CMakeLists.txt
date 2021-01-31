cmake_minimum_required(VERSION 3.17)
project(CppTM)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        AbstractTuringMachine.h TuringMachine.h MetaTuringMachine.h testing/MetaTuringMachine.cpp)

add_executable(CppTM main.cpp ${SOURCES} testing/main.cpp StringStream.h TypeTraits.h)
add_executable(CppTM_Tests testing/main.cpp ${SOURCES})
target_include_directories(CppTM_Tests PUBLIC .)