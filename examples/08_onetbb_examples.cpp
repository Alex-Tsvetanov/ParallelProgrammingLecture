// Example 8: OneTBB Thread Pool and Parallel Algorithms
// Demonstrates Intel Threading Building Blocks (oneTBB)
// Topics: tbb::parallel_for, tbb::parallel_for_each, tbb::task_group
// 
// To compile: g++ -std=c++17 08_onetbb_examples.cpp -ltbb -o tbb_example
// Note: Requires oneTBB library installed

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>

// OneTBB headers
#ifdef __has_include
#  if __has_include(<tbb/parallel_for.h>)
#    include <tbb/parallel_for.h>
#    include <tbb/parallel_for_each.h>
#    include <tbb/task_group.h>
#    include <tbb/blocked_range.h>
#    include <tbb/global_control.h>
#    define HAS_TBB 1
#  else
#    define HAS_TBB 0
#  endif
#else
#  define HAS_TBB 0
#endif

#if HAS_TBB

// Example 1: parallel_for - Simple range iteration
void example_parallel_for() {
    std::cout << "\n=== Example 1: tbb::parallel_for ===\n";
    
    const int N = 20;
    std::vector<double> data(N);
    
    // Initialize data in parallel
    tbb::parallel_for(tbb::blocked_range<int>(0, N),
        [&data](const tbb::blocked_range<int>& r) {
            for (int i = r.begin(); i != r.end(); ++i) {
                data[i] = std::sin(i * 0.1) * 100;
                std::cout << "  [Thread] Processing index " << i 
                         << " = " << data[i] << "\n";
            }
        });
    
    std::cout << "Data initialized in parallel\n";
}

// Example 2: parallel_for_each - Iterate over container
void example_parallel_for_each() {
    std::cout << "\n=== Example 2: tbb::parallel_for_each ===\n";
    
    std::vector<std::string> words = {
        "parallel", "computing", "threading", "oneTBB",
        "scalable", "performance", "concurrent", "algorithms"
    };
    
    std::cout << "Processing words in parallel:\n";
    tbb::parallel_for_each(words.begin(), words.end(),
        [](std::string& word) {
            // Transform to uppercase
            for (char& c : word) {
                c = std::toupper(c);
            }
            std::cout << "  [Thread] Transformed: " << word << "\n";
        });
    
    std::cout << "All words transformed\n";
}

// Example 3: Publisher/Subscriber with TBB
template<typename Event>
class TBBEventBroker {
public:
    using Callback = std::function<void(const Event&)>;

private:
    std::vector<Callback> callbacks;
    std::mutex mutex;

public:
    void subscribe(Callback cb) {
        std::lock_guard<std::mutex> lock(mutex);
        callbacks.push_back(std::move(cb));
    }

    void publish_parallel(const Event& event) {
        std::vector<Callback> local_callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex);
            local_callbacks = callbacks;
        }
        
        // Parallel dispatch using TBB
        tbb::parallel_for_each(local_callbacks.begin(), local_callbacks.end(),
            [&event](const Callback& cb) {
                cb(event);
            });
    }

    size_t subscriber_count() {
        std::lock_guard<std::mutex> lock(mutex);
        return callbacks.size();
    }
};

struct MarketData {
    std::string symbol;
    double price;
};

void example_pubsub_tbb() {
    std::cout << "\n=== Example 3: Publisher/Subscriber with TBB ===\n";
    
    TBBEventBroker<MarketData> broker;
    
    // Add subscribers
    broker.subscribe([](const MarketData& data) {
        std::cout << "  [Strategy 1] Processing " << data.symbol 
                 << " @ $" << data.price << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });
    
    broker.subscribe([](const MarketData& data) {
        std::cout << "  [Risk Engine] Checking " << data.symbol << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    });
    
    broker.subscribe([](const MarketData& data) {
        std::cout << "  [Logger] Recording " << data.symbol 
                 << " = $" << data.price << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    std::cout << "Subscribers registered: " << broker.subscriber_count() << "\n";
    
    // Publish event - subscribers process in parallel
    std::cout << "Publishing event (parallel dispatch)...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    broker.publish_parallel({"AAPL", 175.50});
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Parallel dispatch completed in " << duration.count() << "ms\n";
    std::cout << "(Sequential would take ~90ms, parallel ~50ms with 3 threads)\n";
}

// Example 4: task_group for dynamic task spawning
void example_task_group() {
    std::cout << "\n=== Example 4: tbb::task_group ===\n";
    
    tbb::task_group tasks;
    
    // Spawn multiple tasks dynamically
    for (int i = 0; i < 5; ++i) {
        tasks.run([i]() {
            std::cout << "  [Task " << i << "] Starting\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100 - i * 10));
            std::cout << "  [Task " << i << "] Completed\n";
        });
    }
    
    std::cout << "All tasks spawned, waiting...\n";
    tasks.wait(); // Wait for all tasks to complete
    std::cout << "All tasks completed\n";
}

// Example 5: Benchmark - Sequential vs Parallel
void benchmark() {
    std::cout << "\n=== Example 5: Performance Benchmark ===\n";
    
    const int N = 10000000;
    std::vector<double> data(N);
    
    // Sequential
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        data[i] = std::sqrt(i) * std::sin(i * 0.001);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto seq_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Sequential: " << seq_time.count() << "ms\n";
    
    // Parallel with TBB
    start = std::chrono::high_resolution_clock::now();
    tbb::parallel_for(tbb::blocked_range<int>(0, N),
        [&data](const tbb::blocked_range<int>& r) {
            for (int i = r.begin(); i != r.end(); ++i) {
                data[i] = std::sqrt(i) * std::sin(i * 0.001);
            }
        });
    end = std::chrono::high_resolution_clock::now();
    auto par_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Parallel (TBB): " << par_time.count() << "ms\n";
    std::cout << "Speedup: " << (double)seq_time.count() / par_time.count() << "x\n";
}

int main() {
    std::cout << "=== OneTBB Examples ===\n";
    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << "\n";
    
    // Set number of threads (optional)
    tbb::global_control gc(tbb::global_control::max_allowed_parallelism, 
                          std::thread::hardware_concurrency());
    
    example_parallel_for();
    example_parallel_for_each();
    example_pubsub_tbb();
    example_task_group();
    benchmark();
    
    std::cout << "\n=== All examples completed ===\n";
    
    return 0;
}

#else

int main() {
    std::cout << "OneTBB library not found!\n";
    std::cout << "Please install oneTBB:\n";
    std::cout << "  Ubuntu/Debian: sudo apt-get install libtbb-dev\n";
    std::cout << "  macOS: brew install tbb\n";
    std::cout << "  Or download from: https://github.com/oneapi-src/oneTBB\n";
    return 1;
}

#endif
