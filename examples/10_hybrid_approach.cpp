// Example 10: Hybrid Approach - Thread Pool + Lock-Free Queue + Pub/Sub
// Demonstrates combining multiple techniques for a high-performance system
// Topics: Integration of thread pool, lock-free structures, and event-driven design

#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <vector>
#include <chrono>
#include <string>

// Lock-free SPSC (Single Producer Single Consumer) Queue
template<typename T, size_t Size = 1024>
class SPSCQueue {
private:
    struct alignas(64) { // Cache line alignment
        std::atomic<size_t> value{0};
    } head, tail;
    
    T buffer[Size];

public:
    bool enqueue(const T& item) {
        size_t current_tail = tail.value.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Size;
        
        if (next_tail == head.value.load(std::memory_order_acquire)) {
            return false; // Queue full
        }
        
        buffer[current_tail] = item;
        tail.value.store(next_tail, std::memory_order_release);
        return true;
    }

    bool dequeue(T& item) {
        size_t current_head = head.value.load(std::memory_order_relaxed);
        
        if (current_head == tail.value.load(std::memory_order_acquire)) {
            return false; // Queue empty
        }
        
        item = buffer[current_head];
        head.value.store((current_head + 1) % Size, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head.value.load(std::memory_order_acquire) == 
               tail.value.load(std::memory_order_acquire);
    }
};

// Worker thread for thread pool
class Worker {
private:
    std::thread thread;
    SPSCQueue<std::function<void()>> tasks;
    std::atomic<bool> running{true};

    void run() {
        std::function<void()> task;
        while (running.load(std::memory_order_acquire)) {
            if (tasks.dequeue(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
        
        // Drain remaining tasks
        while (tasks.dequeue(task)) {
            task();
        }
    }

public:
    Worker() : thread(&Worker::run, this) {}

    ~Worker() {
        running.store(false, std::memory_order_release);
        if (thread.joinable()) {
            thread.join();
        }
    }

    bool submit(std::function<void()>&& task) {
        return tasks.enqueue(task);
    }

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
};

// High-performance thread pool with lock-free per-worker queues
class LockFreeThreadPool {
private:
    std::vector<std::unique_ptr<Worker>> workers;
    std::atomic<size_t> next_worker{0};

public:
    explicit LockFreeThreadPool(size_t num_threads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.push_back(std::make_unique<Worker>());
        }
        std::cout << "[ThreadPool] Created with " << num_threads << " workers\n";
    }

    template<typename F>
    bool submit(F&& task) {
        // Round-robin distribution
        size_t start = next_worker.fetch_add(1, std::memory_order_relaxed) % workers.size();
        
        // Try to submit to the selected worker
        for (size_t i = 0; i < workers.size(); ++i) {
            size_t index = (start + i) % workers.size();
            if (workers[index]->submit(std::forward<F>(task))) {
                return true;
            }
        }
        
        return false; // All queues full
    }

    size_t worker_count() const {
        return workers.size();
    }
};

// High-performance event broker using thread pool
template<typename Event>
class HighPerfEventBroker {
public:
    using Callback = std::function<void(const Event&)>;

private:
    struct SubscriberNode {
        Callback callback;
        std::shared_ptr<SubscriberNode> next;
    };

    std::atomic<std::shared_ptr<SubscriberNode>> head{nullptr};
    LockFreeThreadPool& pool;
    std::atomic<uint64_t> events_published{0};
    std::atomic<uint64_t> callbacks_executed{0};

public:
    explicit HighPerfEventBroker(LockFreeThreadPool& thread_pool) 
        : pool(thread_pool) {}

    // Lock-free subscribe
    void subscribe(Callback callback) {
        auto new_node = std::make_shared<SubscriberNode>();
        new_node->callback = std::move(callback);
        
        auto old_head = head.load(std::memory_order_acquire);
        do {
            new_node->next = old_head;
        } while (!head.compare_exchange_weak(
            old_head, new_node,
            std::memory_order_release,
            std::memory_order_acquire));
    }

    // Lock-free publish with parallel dispatch
    void publish(const Event& event) {
        events_published.fetch_add(1, std::memory_order_relaxed);
        
        auto node = head.load(std::memory_order_acquire);
        while (node) {
            // Dispatch each callback to thread pool
            pool.submit([cb = node->callback, event, this]() {
                cb(event);
                callbacks_executed.fetch_add(1, std::memory_order_relaxed);
            });
            node = node->next;
        }
    }

    uint64_t get_events_published() const {
        return events_published.load(std::memory_order_relaxed);
    }

    uint64_t get_callbacks_executed() const {
        return callbacks_executed.load(std::memory_order_relaxed);
    }
};

// Example application: Real-time trading system
struct MarketTick {
    std::string symbol;
    double price;
    uint64_t timestamp;
    int volume;
};

class TradingSystem {
private:
    LockFreeThreadPool pool;
    HighPerfEventBroker<MarketTick> market_broker;

    std::atomic<int> signals_generated{0};
    std::atomic<int> risks_checked{0};
    std::atomic<int> trades_logged{0};

public:
    TradingSystem(size_t threads = std::thread::hardware_concurrency()) 
        : pool(threads), market_broker(pool) {
        
        // Subscribe strategy
        market_broker.subscribe([this](const MarketTick& tick) {
            // Simulate strategy processing
            if (tick.price > 150.0) {
                signals_generated.fetch_add(1, std::memory_order_relaxed);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        });

        // Subscribe risk engine
        market_broker.subscribe([this](const MarketTick& tick) {
            // Simulate risk check
            if (tick.volume > 1000) {
                risks_checked.fetch_add(1, std::memory_order_relaxed);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(15));
        });

        // Subscribe logger
        market_broker.subscribe([this](const MarketTick& tick) {
            // Simulate logging
            trades_logged.fetch_add(1, std::memory_order_relaxed);
            std::this_thread::sleep_for(std::chrono::microseconds(5));
        });

        std::cout << "[TradingSystem] Initialized with " 
                  << pool.worker_count() << " workers\n";
    }

    void process_tick(const MarketTick& tick) {
        market_broker.publish(tick);
    }

    void print_stats() {
        std::cout << "\n=== Trading System Statistics ===\n";
        std::cout << "Events published: " << market_broker.get_events_published() << "\n";
        std::cout << "Callbacks executed: " << market_broker.get_callbacks_executed() << "\n";
        std::cout << "Signals generated: " << signals_generated.load() << "\n";
        std::cout << "Risks checked: " << risks_checked.load() << "\n";
        std::cout << "Trades logged: " << trades_logged.load() << "\n";
    }
};

int main() {
    std::cout << "=== Hybrid Approach: High-Performance Trading System ===\n";
    std::cout << "Combining:\n";
    std::cout << "  - Lock-free thread pool with per-worker queues\n";
    std::cout << "  - Lock-free publisher/subscriber with RCU\n";
    std::cout << "  - Parallel event dispatch\n\n";

    TradingSystem system(4);

    std::cout << "--- Simulating Market Data Feed ---\n";
    auto start = std::chrono::high_resolution_clock::now();

    // Simulate incoming market data
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT", "AMZN"};
    
    for (int i = 0; i < 1000; ++i) {
        MarketTick tick{
            symbols[i % symbols.size()],
            140.0 + (i % 50),
            static_cast<uint64_t>(i),
            500 + (i % 1000)
        };
        
        system.process_tick(tick);
        
        if (i % 250 == 0) {
            std::cout << "  Processed " << i << " ticks...\n";
        }
    }

    std::cout << "  All ticks submitted (non-blocking)\n";
    std::cout << "  Waiting for processing to complete...\n";

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    system.print_stats();
    
    std::cout << "\nProcessing time: " << duration.count() << "ms\n";
    std::cout << "Throughput: " << (1000.0 / duration.count() * 1000) << " ticks/sec\n";

    std::cout << "\n=== Key Benefits of Hybrid Approach ===\n";
    std::cout << "  1. Lock-free queues eliminate contention\n";
    std::cout << "  2. Per-worker queues improve cache locality\n";
    std::cout << "  3. RCU pattern enables wait-free reads\n";
    std::cout << "  4. Thread pool maximizes CPU utilization\n";
    std::cout << "  5. Event-driven design decouples components\n";

    return 0;
}
