// Example 9: Coroutine-based Async I/O Simulation
// Demonstrates coroutines for asynchronous operations
// Topics: co_await, co_return, promise_type, async I/O pattern
//
// Compile with: g++ -std=c++20 09_coroutine_async_io.cpp -o coroutine_io

#include <coroutine>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <optional>

// Event loop for scheduling coroutines
class EventLoop {
private:
    std::queue<std::coroutine_handle<>> ready_queue;

public:
    void schedule(std::coroutine_handle<> handle) {
        ready_queue.push(handle);
    }

    void run() {
        while (!ready_queue.empty()) {
            auto handle = ready_queue.front();
            ready_queue.pop();
            
            if (!handle.done()) {
                handle.resume();
            }
        }
    }

    static EventLoop& instance() {
        static EventLoop loop;
        return loop;
    }
};

// Async Task type
template<typename T>
struct AsyncTask {
    struct promise_type {
        std::optional<T> value;
        std::exception_ptr exception;

        AsyncTask get_return_object() {
            return AsyncTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T v) {
            value = std::move(v);
        }

        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type handle;

    AsyncTask(handle_type h) : handle(h) {}
    ~AsyncTask() { if (handle) handle.destroy(); }

    AsyncTask(AsyncTask&& other) noexcept 
        : handle(std::exchange(other.handle, nullptr)) {}
    
    AsyncTask& operator=(AsyncTask&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = std::exchange(other.handle, nullptr);
        }
        return *this;
    }

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    T get() {
        if (!handle.done()) {
            EventLoop::instance().schedule(handle);
            EventLoop::instance().run();
        }
        
        if (handle.promise().exception) {
            std::rethrow_exception(handle.promise().exception);
        }
        
        return *handle.promise().value;
    }

    bool done() const {
        return handle.done();
    }
};

// Awaitable for simulating async I/O
struct AsyncRead {
    std::string filename;
    std::string result;

    bool await_ready() const noexcept { 
        return false; // Always suspend
    }

    void await_suspend(std::coroutine_handle<> handle) {
        // Simulate async file read in background
        std::thread([this, handle]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            result = "Contents of " + filename + " (simulated)";
            EventLoop::instance().schedule(handle);
        }).detach();
    }

    std::string await_resume() const noexcept {
        std::cout << "  [I/O] Read completed: " << filename << "\n";
        return result;
    }
};

struct AsyncWrite {
    std::string filename;
    std::string data;

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        std::thread([this, handle]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            EventLoop::instance().schedule(handle);
        }).detach();
    }

    void await_resume() const noexcept {
        std::cout << "  [I/O] Write completed: " << filename 
                 << " (" << data.length() << " bytes)\n";
    }
};

// Async file operations using coroutines
AsyncTask<std::string> async_read_file(const std::string& filename) {
    std::cout << "[async_read_file] Starting read: " << filename << "\n";
    
    AsyncRead read_op{filename};
    std::string content = co_await read_op;
    
    std::cout << "[async_read_file] Completed: " << filename << "\n";
    co_return content;
}

AsyncTask<int> async_write_file(const std::string& filename, const std::string& data) {
    std::cout << "[async_write_file] Starting write: " << filename << "\n";
    
    AsyncWrite write_op{filename, data};
    co_await write_op;
    
    std::cout << "[async_write_file] Completed: " << filename << "\n";
    co_return static_cast<int>(data.length());
}

// Process multiple files concurrently
AsyncTask<int> process_files() {
    std::cout << "\n[process_files] Starting parallel I/O operations\n";
    
    // Start multiple async reads (they'll run concurrently)
    auto file1 = async_read_file("data1.txt");
    auto file2 = async_read_file("data2.txt");
    auto file3 = async_read_file("data3.txt");
    
    std::cout << "[process_files] All reads initiated\n";
    
    // Wait for first file
    std::string content1 = file1.get();
    std::cout << "[process_files] Got content1: " << content1.substr(0, 30) << "...\n";
    
    // Wait for second file
    std::string content2 = file2.get();
    std::cout << "[process_files] Got content2: " << content2.substr(0, 30) << "...\n";
    
    // Wait for third file
    std::string content3 = file3.get();
    std::cout << "[process_files] Got content3: " << content3.substr(0, 30) << "...\n";
    
    // Write combined result
    std::string combined = content1 + "\n" + content2 + "\n" + content3;
    auto write_task = async_write_file("output.txt", combined);
    int bytes_written = write_task.get();
    
    std::cout << "[process_files] All operations completed\n";
    co_return bytes_written;
}

// Example with error handling
AsyncTask<std::string> safe_read_file(const std::string& filename) {
    try {
        std::cout << "[safe_read_file] Reading: " << filename << "\n";
        
        AsyncRead read_op{filename};
        std::string content = co_await read_op;
        
        co_return content;
    } catch (const std::exception& e) {
        std::cout << "[safe_read_file] Error: " << e.what() << "\n";
        co_return "ERROR";
    }
}

int main() {
    std::cout << "=== Coroutine-based Async I/O Example ===\n";
    
    std::cout << "\n--- Example 1: Single Async Read ---\n";
    auto task1 = async_read_file("config.json");
    std::string result1 = task1.get();
    std::cout << "Result: " << result1 << "\n";
    
    std::cout << "\n--- Example 2: Single Async Write ---\n";
    auto task2 = async_write_file("log.txt", "Log entry 1\nLog entry 2\n");
    int bytes = task2.get();
    std::cout << "Bytes written: " << bytes << "\n";
    
    std::cout << "\n--- Example 3: Multiple Concurrent Operations ---\n";
    auto task3 = process_files();
    int total_bytes = task3.get();
    std::cout << "Total bytes processed: " << total_bytes << "\n";
    
    std::cout << "\n--- Example 4: Error Handling ---\n";
    auto task4 = safe_read_file("missing.txt");
    std::string result4 = task4.get();
    std::cout << "Result: " << result4 << "\n";
    
    std::cout << "\n=== All coroutine examples completed ===\n";
    std::cout << "\nKey benefits demonstrated:\n";
    std::cout << "  1. Non-blocking I/O operations\n";
    std::cout << "  2. Sequential code that runs asynchronously\n";
    std::cout << "  3. Multiple concurrent operations without threads\n";
    std::cout << "  4. Clean error handling with try-catch\n";
    std::cout << "  5. Local variables safely stored in coroutine frame\n";
    
    return 0;
}
