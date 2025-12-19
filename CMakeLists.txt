cmake_minimum_required(VERSION 3.10)
project(DungeonProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Все исходные файлы
set(SOURCE_FILES
    src/npc.cpp
    src/editor.cpp
    src/factory.cpp
    src/observer.cpp
    src/dragon.cpp
    src/knight.cpp
    src/pegasus.cpp  # ДОБАВЛЕНО
)

# Основное приложение - называется main
add_executable(main main.cpp ${SOURCE_FILES})

# Для многопоточности
if(UNIX)
    target_link_libraries(main pthread)
endif()

# Тесты с Google Test (скачиваем автоматически)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Тесты - называются tests
add_executable(tests tests/tests.cpp ${SOURCE_FILES})
target_link_libraries(tests gtest gtest_main)
if(UNIX)
    target_link_libraries(tests pthread)
endif()

# Для запуска тестов через ctest
enable_testing()
add_test(NAME DungeonTests COMMAND tests)