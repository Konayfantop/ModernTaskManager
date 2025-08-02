cmake_minimum_required(VERSION 3.28.3)
project(ModernTaskManager)

# Manually list all .cpp files — no GLOB confusion
add_executable(out
    src/main.cpp
    src/proc/ProcessInfo.cpp
    src/utils/LogTrace.cpp
)

# This matches your working include path
target_include_directories(out PRIVATE ${CMAKE_SOURCE_DIR}/include/proc)
target_include_directories(out PRIVATE ${CMAKE_SOURCE_DIR}/include/utils)

set_target_properties(out PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)