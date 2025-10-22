// Example 1: Lock-based Thread Pool
// Demonstrates basic thread pool with mutex-protected queue
// Topics: std::thread, std::mutex, std::condition_variable, std::queue

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <chrono>
#include <sstream>

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
            workers.emplace_back([this, i] {
                {
                    std::stringstream ss;
                    ss << "Worker " << i << " started\n";
                    std::cout << ss.str() << std::flush;
                }
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        
                        if (stop && tasks.empty()) {
                            {
                                std::stringstream ss;
                                ss << "Worker " << i << " stopping\n";
                                std::cout << ss.str() << std::flush;
                            }
                            return;
                        }
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task(); // Execute outside the lock
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
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

// Example usage
void cpu_intensive_task(int id, int duration_ms) {
    {
        std::stringstream ss;
        ss << "Task " << id << " starting (duration: " << duration_ms << "ms)\n";
        std::cout << ss.str() << std::flush;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    {
        std::stringstream ss;
        ss << "Task " << id << " completed\n";
        std::cout << ss.str() << std::flush;
    }
}

int main() {
    {
        std::stringstream ss;
        ss << "=== Lock-based Thread Pool Example ===\n";
        ss << "Hardware concurrency: " << std::thread::hardware_concurrency() << "\n\n";
        std::cout << ss.str() << std::flush;
    }

    ThreadPool pool(4);

    // Enqueue 10 tasks
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            cpu_intensive_task(i, 100 + (i % 3) * 50);
        });
    }

    {
        std::stringstream ss;
        ss << "\nAll tasks enqueued. Waiting for completion...\n";
        std::cout << ss.str() << std::flush;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        std::stringstream ss;
        ss << "\nMain thread exiting (pool destructor will wait for workers)\n";
        std::cout << ss.str() << std::flush;
    }

    return 0;
}
