cmake_minimum_required(VERSION 3.15)
project(ParallelProgrammingExamples CXX)

# Set C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find threads package
find_package(Threads REQUIRED)

# Optional: Find TBB
find_package(TBB QUIET)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Helper function to add examples
function(add_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE Threads::Threads)

    # Parse additional arguments
    set(options REQUIRES_CPP20 REQUIRES_TBB)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(ARG_REQUIRES_CPP20)
        target_compile_features(${name} PRIVATE cxx_std_20)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${name} PRIVATE -fcoroutines)
        endif()
    endif()

    if(ARG_REQUIRES_TBB)
        if(TBB_FOUND)
            target_link_libraries(${name} PRIVATE TBB::tbb)
            message(STATUS "Building ${name} with TBB support")
        else()
            message(WARNING "${name} requires TBB but it was not found. Skipping.")
            set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
        endif()
    endif()
endfunction()

# C++17 Examples
add_example(01_thread_pool_lock_based 01_thread_pool_lock_based.cpp)
add_example(02_lock_free_queue 02_lock_free_queue.cpp)
add_example(04_pubsub_synchronous 04_pubsub_synchronous.cpp)
add_example(05_pubsub_async_threadpool 05_pubsub_async_threadpool.cpp)
add_example(06_pubsub_lockfree_rcu 06_pubsub_lockfree_rcu.cpp)
add_example(07_atomic_memory_ordering 07_atomic_memory_ordering.cpp)
add_example(10_hybrid_approach 10_hybrid_approach.cpp)

# C++20 Examples (Coroutines)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.0)
    set(HAS_CPP20_SUPPORT TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
    set(HAS_CPP20_SUPPORT TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29)
    set(HAS_CPP20_SUPPORT TRUE)
else()
    set(HAS_CPP20_SUPPORT FALSE)
endif()

if(HAS_CPP20_SUPPORT)
    message(STATUS "C++20 support detected - building coroutine examples")
    add_example(03_basic_coroutine 03_basic_coroutine.cpp REQUIRES_CPP20)

    # Create test data files for coroutine async I/O example
    file(WRITE "${CMAKE_BINARY_DIR}/data1.txt" "Sample data from file 1\nLine 2 of file 1\n")
    file(WRITE "${CMAKE_BINARY_DIR}/data2.txt" "Sample data from file 2\nLine 2 of file 2\n")
    file(WRITE "${CMAKE_BINARY_DIR}/data3.txt" "Sample data from file 3\nLine 2 of file 3\n")
    file(WRITE "${CMAKE_BINARY_DIR}/config.json" "{\"setting1\": \"value1\", \"setting2\": 42}\n")

    add_example(09_coroutine_async_io 09_coroutine_async_io.cpp REQUIRES_CPP20)
    add_example(coroutine_based_thread_pool coroutine_based_thread_pool.cpp REQUIRES_CPP20)
else()
    message(WARNING "C++20 not supported by compiler - skipping coroutine examples")
    message(STATUS "Requires: GCC 10+, Clang 11+, or MSVC 19.29+")
endif()

# OneTBB Example
add_example(08_onetbb_examples 08_onetbb_examples.cpp REQUIRES_TBB)

# Print summary
message(STATUS "")
message(STATUS "=== Build Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "C++20 Support: ${HAS_CPP20_SUPPORT}")
message(STATUS "TBB Found: ${TBB_FOUND}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "====================")
message(STATUS "")
