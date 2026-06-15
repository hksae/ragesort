cmake_minimum_required(VERSION 3.10)
project(ragesort VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Force Release build type by default if not specified (critical for benchmarks)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

include(GNUInstallDirs)

add_library(ragesort STATIC
    src/ragesort.c
)

target_include_directories(ragesort PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Enable aggressive hardware optimizations based on the compiler
if(MSVC)
    target_compile_options(ragesort PRIVATE /O2 /Oi /Ot /GL)
    target_link_options(ragesort PRIVATE /LTCG)
else()
    # -O3 turns on loops vectorization, -march=native enables CPU-specific instructions (AVX/SSE)
    target_compile_options(ragesort PRIVATE -O3 -march=native -flto -Wall -Wextra)
    target_link_options(ragesort PRIVATE -flto)
endif()

# Executables
add_executable(test_correctness tests/test_correctness.c)
target_link_libraries(test_correctness ragesort)

add_executable(benchmark tests/benchmark.c)
target_link_libraries(benchmark ragesort)

add_executable(demo examples/demo.c)
target_link_libraries(demo ragesort)

# Testing architecture
enable_testing()
add_test(NAME correctness COMMAND test_correctness)

# Installation targets
install(TARGETS ragesort
    EXPORT ragesort-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES include/ragesort.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
