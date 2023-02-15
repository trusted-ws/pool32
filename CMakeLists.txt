cmake_minimum_required(VERSION 3.5)

project(Pool32 VERSION 1.0)
set(CMAKE_C_STANDARD 11)

set(SOURCES
    src/main.c
    src/pool.c
    src/hexdump.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}src/include
)

add_executable(pool32 ${SOURCES})

add_subdirectory(test/)

enable_testing()

add_executable(pool_test test/test_main.c src/pool.c)

add_test(NAME PoolTest COMMAND pool_test)