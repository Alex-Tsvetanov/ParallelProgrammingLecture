# Parallel Programming Lecture Examples

[![Build Examples](https://github.com/Alex-Tsvetanov/ParallelProgrammingLecture/actions/workflows/build-examples.yml/badge.svg)](https://github.com/Alex-Tsvetanov/ParallelProgrammingLecture/actions/workflows/build-examples.yml)

This directory contains practical C++ examples demonstrating modern parallelism and concurrency techniques covered in the lecture.

## Examples Overview

### 1. Thread Pools
- **01_thread_pool_lock_based.cpp** - Basic thread pool using `std::mutex` and `std::condition_variable`
- **coroutine_based_thread_pool.cpp** - Advanced thread pool with C++20 coroutines

### 2. Lock-Free Data Structures
- **02_lock_free_queue.cpp** - Lock-free queue using `std::atomic` and CAS operations
- **07_atomic_memory_ordering.cpp** - Comprehensive atomic operations and memory ordering examples

### 3. Coroutines (C++20)
- **03_basic_coroutine.cpp** - Introduction to coroutines with `co_await` and `co_return`
- **09_coroutine_async_io.cpp** - Async I/O simulation using coroutines

### 4. Publisher/Subscriber Pattern
- **04_pubsub_synchronous.cpp** - Basic synchronous pub/sub implementation
- **05_pubsub_async_threadpool.cpp** - Async pub/sub with thread pool for parallel dispatch
- **06_pubsub_lockfree_rcu.cpp** - Lock-free pub/sub using Read-Copy-Update (RCU) pattern

### 5. OneTBB (Intel Threading Building Blocks)
- **08_onetbb_examples.cpp** - Parallel algorithms with oneTBB library

### 6. Full Project
- **pubsub-lib/** - Complete publisher/subscriber library with benchmarks

## Compilation Instructions

### Option 1: Using Docker (Easiest - No Setup Required!)

```bash
# Quick start with Docker Compose
docker-compose up -d
docker-compose exec parallel-examples bash

# Inside container, all examples are pre-built and ready to run!
examples        # List all examples
run-all         # Run all examples
benchmark       # Performance benchmarks
```

**See [DOCKER.md](DOCKER.md) for complete Docker documentation.**

### Option 2: Local Compilation

#### Prerequisites
- C++20 compiler (GCC 10+, Clang 11+, MSVC 2019+)
- CMake 3.15+ (optional, for building all examples)
- OneTBB library (for example 08)

### Install Dependencies

#### Docker (Recommended):
```bash
# Install Docker Desktop (includes Docker Compose)
# Mac: https://docs.docker.com/desktop/install/mac-install/
# Windows: https://docs.docker.com/desktop/install/windows-install/
# Linux: https://docs.docker.com/desktop/install/linux-install/

# Or install Docker Engine + Docker Compose separately on Linux
sudo apt-get install docker.io docker-compose
```

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libtbb-dev
```

#### macOS:
```bash
brew install cmake tbb
```

### Compile Individual Examples

#### Basic examples (C++17):
```bash
# Lock-based thread pool
g++ -std=c++17 -pthread 01_thread_pool_lock_based.cpp -o thread_pool

# Lock-free queue
g++ -std=c++17 -pthread 02_lock_free_queue.cpp -o lock_free_queue

# Synchronous pub/sub
g++ -std=c++17 -pthread 04_pubsub_synchronous.cpp -o pubsub_sync

# Async pub/sub with thread pool
g++ -std=c++17 -pthread 05_pubsub_async_threadpool.cpp -o pubsub_async

# Atomic operations
g++ -std=c++17 -pthread 07_atomic_memory_ordering.cpp -o atomic_demo
```

#### Coroutine examples (C++20):
```bash
# Basic coroutine
g++ -std=c++20 -pthread 03_basic_coroutine.cpp -o basic_coroutine

# Coroutine-based thread pool
g++ -std=c++20 -pthread coroutine_based_thread_pool.cpp -o coro_pool

# Async I/O with coroutines
g++ -std=c++20 -pthread 09_coroutine_async_io.cpp -o async_io
```

#### OneTBB example:
```bash
g++ -std=c++17 -pthread 08_onetbb_examples.cpp -ltbb -o tbb_example
```

### Compile All Examples with CMake

```bash
mkdir build
cd build
cmake ..
make
```

Executables will be in the `build` directory.

## Running Examples

After compilation, run any example:
```bash
./thread_pool
./lock_free_queue
./pubsub_sync
# etc.
```

## Key Concepts Demonstrated

### Threading and Synchronization
- `std::thread` - Creating and managing threads
- `std::mutex` - Mutual exclusion locks
- `std::condition_variable` - Thread synchronization
- `std::unique_lock` / `std::lock_guard` - RAII lock management

### Atomic Operations
- `std::atomic<T>` - Lock-free atomic variables
- `compare_exchange_weak/strong` - Compare-And-Swap (CAS)
- `memory_order_*` - Memory ordering guarantees
- `std::atomic_flag` - Lock-free boolean flag

### Coroutines (C++20)
- `co_await` - Suspend and wait for result
- `co_return` - Return value from coroutine
- `std::coroutine_handle` - Handle to coroutine state
- `promise_type` - Coroutine promise customization
- Coroutine frame - Heap-allocated state

### Design Patterns
- **Thread Pool** - Worker threads processing task queue
- **Publisher/Subscriber** - Event-driven architecture
- **Lock-Free** - Non-blocking concurrent data structures
- **RCU (Read-Copy-Update)** - Optimistic concurrency control

## Performance Characteristics

| Example | Technique | Pros | Cons |
|---------|-----------|------|------|
| 01 | Lock-based pool | Simple, easy to debug | Contention under high load |
| 02 | Lock-free queue | High throughput | Complex, ABA problem |
| 04 | Sync pub/sub | Predictable, simple | Blocking subscribers |
| 05 | Async pub/sub | Non-blocking publish | Thread overhead |
| 06 | Lock-free RCU | Wait-free reads | Memory overhead |
| 08 | OneTBB | Auto load-balancing | External dependency |

## Troubleshooting

### Compiler doesn't support C++20
- For coroutine examples, ensure GCC 10+, Clang 11+, or MSVC 2019+
- Use `-std=c++20` or `-std=c++2a` flag

### Missing TBB library
- Install oneTBB: `sudo apt-get install libtbb-dev` (Ubuntu)
- Or download from: https://github.com/oneapi-src/oneTBB

### Compilation errors
- Check C++ standard: `g++ --version` or `clang++ --version`
- Ensure pthread support: `-pthread` flag
- For coroutines, some compilers need: `-fcoroutines`

## Related Topics

These examples complement the lecture covering:
1. **Thread Pools** - Efficient task parallelization
2. **Coroutines** - Lightweight async operations
3. **Publisher/Subscriber** - Decoupled event systems
4. **Lock-free structures** - High-performance concurrency
5. **Hybrid approaches** - Combining multiple techniques

## References

- C++ Concurrency in Action (Anthony Williams)
- The Art of Multiprocessor Programming (Herlihy & Shavit)
- https://en.cppreference.com - C++ standard library reference
- https://preshing.com - Lock-free programming blog

## Contact

For questions about these examples:
- Email: atsvetanov@tu-sofia.bg
- GitHub: https://github.com/Alex-Tsvetanov
