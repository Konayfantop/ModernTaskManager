cmake_minimum_required(VERSION 3.28.3)
project(ModernTaskManager)

# declare testing which 
option(BUILD_TESTING "Enable test builds" OFF)

# Manually list all .cpp files — no GLOB confusion
add_executable(out
    src/main.cpp
    src/proc/ProcessInfo.cpp
)

# This matches your working include path
target_include_directories(out PRIVATE ${CMAKE_SOURCE_DIR}/include/proc)
target_include_directories(out PRIVATE ${CMAKE_SOURCE_DIR}/include/utils)

if(BUILD_TESTING)
    enable_testing()

    # Download GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(googletest)

    # Add all test sources
    set(TEST_SOURCES
        test/main.cpp
        test/proc/ProcessInfoTest.cpp
    )

    add_executable(my_tests ${TEST_SOURCES})

    target_sources(my_tests PRIVATE src/proc/ProcessInfo.cpp)
    target_include_directories(my_tests PRIVATE ${CMAKE_SOURCE_DIR}/include/proc)
    target_include_directories(my_tests PRIVATE ${CMAKE_SOURCE_DIR}/include/utils)
    target_link_libraries(my_tests PRIVATE gtest gtest_main)

    include(GoogleTest)
    gtest_discover_tests(my_tests)
endif()

set_target_properties(out PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)