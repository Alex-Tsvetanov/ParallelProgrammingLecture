// Example 7: Atomic Operations and Memory Ordering
// Demonstrates different memory ordering semantics
// Topics: std::atomic, memory_order variants, synchronization

#include <atomic>
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <mutex>

// Example 1: Sequentially Consistent (default)
class SequentiallyConsistent {
private:
    std::atomic<int> data{0};
    std::atomic<bool> ready{false};

public:
    void producer() {
        data.store(42);  // default: memory_order_seq_cst
        ready.store(true);
        std::cout << "[Producer] Data written and ready flag set\n";
    }

    void consumer() {
        while (!ready.load()) {  // default: memory_order_seq_cst
            std::this_thread::yield();
        }
        int value = data.load();
        std::cout << "[Consumer] Read data: " << value << "\n";
        assert(value == 42);
    }
};

// Example 2: Acquire-Release Semantics
class AcquireRelease {
private:
    std::atomic<int> data{0};
    std::atomic<bool> ready{false};

public:
    void producer() {
        data.store(100, std::memory_order_relaxed);  // No ordering
        ready.store(true, std::memory_order_release); // Release: makes data visible
        std::cout << "[Producer AR] Data released\n";
    }

    void consumer() {
        while (!ready.load(std::memory_order_acquire)) { // Acquire: sees released data
            std::this_thread::yield();
        }
        int value = data.load(std::memory_order_relaxed);
        std::cout << "[Consumer AR] Acquired data: " << value << "\n";
        assert(value == 100);
    }
};

// Example 3: Compare-And-Swap for Lock-Free Counter
class LockFreeCounter {
private:
    std::atomic<int> count{0};

public:
    void increment() {
        int expected = count.load(std::memory_order_relaxed);
        while (!count.compare_exchange_weak(
            expected,
            expected + 1,
            std::memory_order_release,
            std::memory_order_relaxed)) {
            // Retry if another thread modified count
        }
    }

    int get() const {
        return count.load(std::memory_order_acquire);
    }
};

// Example 4: Spinlock using atomic flag
class Spinlock {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // Spin until we acquire the lock
            std::this_thread::yield();
        }
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// Example 5: Double-Checked Locking (lazy initialization)
class Singleton {
private:
    static std::atomic<Singleton*> instance;
    static std::mutex mutex;

    Singleton() {
        std::cout << "  [Singleton] Instance created\n";
    }

public:
    static Singleton* get_instance() {
        Singleton* tmp = instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            tmp = instance.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new Singleton();
                instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }
};

std::atomic<Singleton*> Singleton::instance{nullptr};
std::mutex Singleton::mutex;

// Test functions
void test_sequential_consistency() {
    std::cout << "\n=== Test 1: Sequential Consistency ===\n";
    SequentiallyConsistent sc;
    std::thread t1(&SequentiallyConsistent::producer, &sc);
    std::thread t2(&SequentiallyConsistent::consumer, &sc);
    t1.join();
    t2.join();
}

void test_acquire_release() {
    std::cout << "\n=== Test 2: Acquire-Release ===\n";
    AcquireRelease ar;
    std::thread t1(&AcquireRelease::producer, &ar);
    std::thread t2(&AcquireRelease::consumer, &ar);
    t1.join();
    t2.join();
}

void test_lock_free_counter() {
    std::cout << "\n=== Test 3: Lock-Free Counter ===\n";
    LockFreeCounter counter;
    
    std::vector<std::thread> threads;
    const int num_threads = 10;
    const int increments_per_thread = 1000;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&counter, increments_per_thread]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                counter.increment();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Expected: " << (num_threads * increments_per_thread) << "\n";
    std::cout << "Actual: " << counter.get() << "\n";
}

void test_spinlock() {
    std::cout << "\n=== Test 4: Spinlock ===\n";
    Spinlock lock;
    int shared_data = 0;

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&lock, &shared_data, i]() {
            lock.lock();
            std::cout << "  [Thread " << i << "] Acquired lock\n";
            shared_data++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "  [Thread " << i << "] Releasing lock\n";
            lock.unlock();
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    std::cout << "Shared data: " << shared_data << "\n";
}

void test_singleton() {
    std::cout << "\n=== Test 5: Double-Checked Locking Singleton ===\n";
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i]() {
            auto* instance = Singleton::get_instance();
            std::cout << "  [Thread " << i << "] Got instance: " << instance << "\n";
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    std::cout << "=== Atomic Operations and Memory Ordering Examples ===\n";

    test_sequential_consistency();
    test_acquire_release();
    test_lock_free_counter();
    test_spinlock();
    test_singleton();

    std::cout << "\n=== All tests completed ===\n";
    std::cout << "\nKey concepts demonstrated:\n";
    std::cout << "  1. Sequential consistency (strongest, slowest)\n";
    std::cout << "  2. Acquire-Release (synchronization without seq_cst overhead)\n";
    std::cout << "  3. Compare-and-swap (lock-free algorithms)\n";
    std::cout << "  4. Spinlock (busy-waiting synchronization)\n";
    std::cout << "  5. Double-checked locking (lazy initialization)\n";

    return 0;
}
