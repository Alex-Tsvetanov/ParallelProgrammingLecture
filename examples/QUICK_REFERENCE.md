# Parallel Programming Examples - Quick Reference

## Examples by Topic

### Thread Pools
| File | Level | Concepts |
|------|-------|----------|
| `01_thread_pool_lock_based.cpp` | Beginner | `std::thread`, `std::mutex`, `std::condition_variable`, basic queue |
| `coroutine_based_thread_pool.cpp` | Advanced | C++20 coroutines, `co_await`, suspend/resume, coroutine frame |

**Use when:** You need to execute many short-lived tasks efficiently

### Lock-Free Data Structures
| File | Level | Concepts |
|------|-------|----------|
| `02_lock_free_queue.cpp` | Intermediate | `std::atomic`, CAS, `compare_exchange_weak`, memory ordering |
| `07_atomic_memory_ordering.cpp` | Intermediate | Memory barriers, acquire/release, sequential consistency |

**Use when:** Minimizing contention is critical (high-throughput systems)

### Coroutines (C++20)
| File | Level | Concepts |
|------|-------|----------|
| `03_basic_coroutine.cpp` | Beginner | `co_await`, `co_return`, `promise_type`, basic suspension |
| `09_coroutine_async_io.cpp` | Intermediate | Async I/O pattern, event loop, resumable functions |

**Use when:** You have many I/O-bound or wait-heavy operations

### Publisher/Subscriber
| File | Level | Concepts |
|------|-------|----------|
| `04_pubsub_synchronous.cpp` | Beginner | Callbacks, `std::function`, event-driven design |
| `05_pubsub_async_threadpool.cpp` | Intermediate | Thread pool integration, parallel dispatch |
| `06_pubsub_lockfree_rcu.cpp` | Advanced | RCU pattern, lock-free subscription, wait-free publish |

**Use when:** Decoupling components or building event-driven systems

### OneTBB
| File | Level | Concepts |
|------|-------|----------|
| `08_onetbb_examples.cpp` | Intermediate | `parallel_for`, `parallel_for_each`, `task_group` |

**Use when:** You want automatic work-stealing and load balancing

### Hybrid Approaches
| File | Level | Concepts |
|------|-------|----------|
| `10_hybrid_approach.cpp` | Advanced | Combining thread pool + lock-free + pub/sub |

**Use when:** Building high-performance production systems

## Quick Compilation Guide

### Single File (C++17)
```bash
g++ -std=c++17 -pthread -O2 <filename>.cpp -o <output>
```

### Single File (C++20 Coroutines)
```bash
g++ -std=c++20 -pthread -O2 <filename>.cpp -o <output>
# or for Clang:
clang++ -std=c++20 -pthread -fcoroutines -O2 <filename>.cpp -o <output>
```

### With OneTBB
```bash
g++ -std=c++17 -pthread -O2 08_onetbb_examples.cpp -ltbb -o tbb_example
```

### All Examples (CMake)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Complexity Comparison

| Approach | Implementation | Performance | Scalability |
|----------|---------------|-------------|-------------|
| Lock-based | ⭐⭐⭐⭐⭐ Easy | ⭐⭐⭐ Good | ⭐⭐⭐ Moderate |
| Lock-free | ⭐⭐ Hard | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐⭐ Excellent |
| Coroutines | ⭐⭐⭐ Moderate | ⭐⭐⭐⭐ Very Good | ⭐⭐⭐⭐ Good |
| OneTBB | ⭐⭐⭐⭐ Easy | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐⭐⭐ Excellent |

## Performance Tips

### Thread Pools
- Size = `std::thread::hardware_concurrency()` for CPU-bound tasks
- Size = 2-4x cores for I/O-bound tasks
- Avoid creating pools dynamically

### Lock-Free
- Profile first - complexity isn't always worth it
- Watch for ABA problem (use tagged pointers or hazard pointers)
- Memory ordering matters: use acquire/release, not seq_cst unless needed

### Coroutines
- Great for I/O, avoid for CPU-intensive work
- Local variables are safe (coroutine frame)
- Watch for dangling references to external data

### Pub/Sub
- Async dispatch for long-running subscribers
- Consider batching events for high throughput
- Lock-free for ultra-low latency

## Common Patterns

### Pattern 1: CPU-Intensive Batch Processing
```
Thread Pool (lock-based) + parallel_for
```
**Example:** Image processing, data transformation

### Pattern 2: High-Throughput Event System
```
Lock-Free Queue + Thread Pool + Pub/Sub
```
**Example:** Trading systems, real-time analytics

### Pattern 3: Async I/O Server
```
Coroutines + Event Loop
```
**Example:** Web servers, database connections

### Pattern 4: Mixed Workload
```
Coroutines (I/O) + Thread Pool (CPU) + Pub/Sub (events)
```
**Example:** Game engines, full-stack applications

## Debugging Tips

### Thread Issues
- Use ThreadSanitizer: `-fsanitize=thread`
- Helgrind: `valgrind --tool=helgrind ./program`
- GDB thread commands: `info threads`, `thread <n>`

### Atomics
- Check memory ordering carefully
- Use atomic ref/ub sanitizer: `-fsanitize=address,undefined`
- Print loads/stores for debugging

### Coroutines
- Use coroutine-aware debuggers (LLDB, VS)
- Add logging at suspend/resume points
- Check promise_type implementations

## When to Use What?

| Scenario | Best Choice | Why |
|----------|-------------|-----|
| 1000 short tasks | Thread Pool (lock-based) | Simple, effective |
| Million tiny tasks | OneTBB | Auto work-stealing |
| Ultra-low latency | Lock-free structures | No blocking |
| Network I/O | Coroutines | Lightweight async |
| Event notification | Pub/Sub | Decoupling |
| Real-time trading | Hybrid (10) | Max performance |

## Example Workflow

For a real application:

1. **Start Simple**: Lock-based thread pool (`01`)
2. **Profile**: Find bottlenecks
3. **Optimize Hot Paths**: 
   - High contention → Lock-free (`02`, `06`)
   - I/O waits → Coroutines (`03`, `09`)
   - CPU-bound → OneTBB (`08`)
4. **Integrate**: Combine techniques (`10`)
5. **Measure**: Benchmark before/after

## Learning Path

### Beginner
1. `01_thread_pool_lock_based.cpp` - Basic threading
2. `04_pubsub_synchronous.cpp` - Event patterns
3. `03_basic_coroutine.cpp` - Async programming

### Intermediate
4. `02_lock_free_queue.cpp` - Atomics introduction
5. `05_pubsub_async_threadpool.cpp` - Integration
6. `07_atomic_memory_ordering.cpp` - Memory model
7. `08_onetbb_examples.cpp` - Production library

### Advanced
8. `06_pubsub_lockfree_rcu.cpp` - Advanced lock-free
9. `09_coroutine_async_io.cpp` - Complex coroutines
10. `10_hybrid_approach.cpp` - System design

## Resources

- **Book**: "C++ Concurrency in Action" by Anthony Williams
- **Web**: https://en.cppreference.com (std::thread, std::atomic)
- **Blog**: https://preshing.com (lock-free programming)
- **Video**: CppCon talks on parallelism and concurrency

## License

Examples are provided for educational purposes as part of the Parallel Programming course at TU-Sofia.
