// Example 5: Async Publisher/Subscriber with Thread Pool
// Combines Thread Pool + Pub/Sub for parallel event processing
// Topics: std::thread, std::mutex, std::function, async dispatch

#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

// Simple Thread Pool (reused from earlier examples)
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};

// Async Event Broker
template<typename Event>
class AsyncEventBroker {
public:
    using Callback = std::function<void(const Event&)>;
    
private:
    std::vector<Callback> subscribers;
    ThreadPool& pool;
    std::mutex mutex;

public:
    explicit AsyncEventBroker(ThreadPool& thread_pool) : pool(thread_pool) {}

    void subscribe(Callback callback) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.push_back(std::move(callback));
    }

    void publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "[AsyncBroker] Publishing to " << subscribers.size() 
                  << " subscribers (parallel)\n";
        
        // Each subscriber gets processed in parallel
        for (const auto& subscriber : subscribers) {
            pool.enqueue([subscriber, event]() {
                subscriber(event);
            });
        }
        // Note: publish() returns immediately, doesn't wait for processing
    }
};

// Example event
struct StockPrice {
    std::string symbol;
    double price;
    long timestamp;
};

// Subscribers with different processing times
class RiskEngine {
public:
    void process(const StockPrice& stock) {
        auto tid = std::this_thread::get_id();
        std::cout << "  [RiskEngine, thread " << tid << "] Analyzing " 
                  << stock.symbol << " @ $" << stock.price << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "  [RiskEngine, thread " << tid << "] Analysis complete\n";
    }
};

class TradingStrategy {
public:
    void process(const StockPrice& stock) {
        auto tid = std::this_thread::get_id();
        std::cout << "  [TradingStrategy, thread " << tid << "] Evaluating " 
                  << stock.symbol << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "  [TradingStrategy, thread " << tid << "] Decision made\n";
    }
};

class DataRecorder {
public:
    void process(const StockPrice& stock) {
        auto tid = std::this_thread::get_id();
        std::cout << "  [DataRecorder, thread " << tid << "] Recording " 
                  << stock.symbol << " = $" << stock.price << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
};

int main() {
    std::cout << "=== Async Publisher/Subscriber with Thread Pool ===\n\n";

    // Create thread pool
    ThreadPool pool(4);
    std::cout << "Thread pool created with 4 workers\n\n";

    // Create async event broker
    AsyncEventBroker<StockPrice> broker(pool);

    // Create and register subscribers
    RiskEngine risk;
    TradingStrategy strategy;
    DataRecorder recorder;

    broker.subscribe([&risk](const StockPrice& s) { risk.process(s); });
    broker.subscribe([&strategy](const StockPrice& s) { strategy.process(s); });
    broker.subscribe([&recorder](const StockPrice& s) { recorder.process(s); });

    std::cout << "Subscribers registered\n\n";

    // Publish events
    std::cout << "--- Publishing AAPL ---\n";
    broker.publish({"AAPL", 175.50, 1234567890});
    
    std::cout << "\n--- Publishing GOOGL ---\n";
    broker.publish({"GOOGL", 140.25, 1234567891});

    std::cout << "\n--- Publishing MSFT ---\n";
    broker.publish({"MSFT", 380.00, 1234567892});

    std::cout << "\n[Main] All events published (non-blocking)\n";
    std::cout << "[Main] Waiting for processing to complete...\n\n";

    // Wait for all processing to complete
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\n[Main] Exiting (pool will cleanup)\n";

    return 0;
}
