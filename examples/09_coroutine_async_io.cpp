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
#include <mutex>
#include <sstream>

// Event loop for scheduling coroutines
class EventLoop {
private:
    std::queue<std::coroutine_handle<>> ready_queue;
    std::mutex queue_mutex;

public:
    void schedule(std::coroutine_handle<> handle) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        ready_queue.push(handle);
    }

    void run() {
        while (true) {
            std::coroutine_handle<> handle;
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (ready_queue.empty()) {
                    break;
                }
                handle = ready_queue.front();
                ready_queue.pop();
            }
            
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
        // Start the coroutine if not already started
        if (!handle.done()) {
            EventLoop::instance().schedule(handle);
        }
        
        // Keep running the event loop until this coroutine completes
        while (!handle.done()) {
            EventLoop::instance().run();
            if (!handle.done()) {
                // Brief sleep to allow async operations to complete
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
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
    mutable std::string result;

    bool await_ready() const noexcept { 
        return false; // Always suspend
    }

    void await_suspend(std::coroutine_handle<> handle) const {
        // Capture filename by value and use shared pointer for result
        std::string fname = filename;
        auto result_ptr = std::make_shared<std::string>();
        
        std::thread([fname, result_ptr, handle, this]() mutable {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            *result_ptr = "Contents of " + fname + " (simulated)";
            // Store result back
            const_cast<AsyncRead*>(this)->result = *result_ptr;
            EventLoop::instance().schedule(handle);
        }).detach();
    }

    std::string await_resume() const noexcept {
        std::stringstream ss;
        ss << "  [I/O] Read completed: " << filename << "\n";
        std::cout << ss.str() << std::flush;
        return result;
    }
};

struct AsyncWrite {
    std::string filename;
    std::string data;

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) const {
        // Capture by value to avoid lifetime issues
        std::string fname = filename;
        size_t data_len = data.length();
        
        std::thread([handle, fname, data_len]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            EventLoop::instance().schedule(handle);
        }).detach();
    }

    void await_resume() const noexcept {
        std::stringstream ss;
        ss << "  [I/O] Write completed: " << filename 
           << " (" << data.length() << " bytes)\n";
        std::cout << ss.str() << std::flush;
    }
};

// Async file operations using coroutines
AsyncTask<std::string> async_read_file(const std::string& filename) {
    {
        std::stringstream ss;
        ss << "[async_read_file] Starting read: " << filename << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    AsyncRead read_op{filename};
    std::string content = co_await read_op;
    
    {
        std::stringstream ss;
        ss << "[async_read_file] Completed: " << filename << "\n";
        std::cout << ss.str() << std::flush;
    }
    co_return content;
}

AsyncTask<int> async_write_file(const std::string& filename, const std::string& data) {
    {
        std::stringstream ss;
        ss << "[async_write_file] Starting write: " << filename << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    AsyncWrite write_op{filename, data};
    co_await write_op;
    
    {
        std::stringstream ss;
        ss << "[async_write_file] Completed: " << filename << "\n";
        std::cout << ss.str() << std::flush;
    }
    co_return static_cast<int>(data.length());
}

// Process multiple files concurrently
AsyncTask<int> process_files() {
    {
        std::stringstream ss;
        ss << "\n[process_files] Starting parallel I/O operations\n";
        std::cout << ss.str() << std::flush;
    }
    
    // Start multiple async reads (they'll run concurrently)
    auto file1 = async_read_file(std::string("data1.txt"));
    auto file2 = async_read_file(std::string("data2.txt"));
    auto file3 = async_read_file(std::string("data3.txt"));
    
    {
        std::stringstream ss;
        ss << "[process_files] All reads initiated\n";
        std::cout << ss.str() << std::flush;
    }
    
    // Wait for first file
    std::string content1 = file1.get();
    {
        std::stringstream ss;
        ss << "[process_files] Got content1: " << content1.substr(0, 30) << "...\n";
        std::cout << ss.str() << std::flush;
    }
    
    // Wait for second file
    std::string content2 = file2.get();
    {
        std::stringstream ss;
        ss << "[process_files] Got content2: " << content2.substr(0, 30) << "...\n";
        std::cout << ss.str() << std::flush;
    }
    
    // Wait for third file
    std::string content3 = file3.get();
    {
        std::stringstream ss;
        ss << "[process_files] Got content3: " << content3.substr(0, 30) << "...\n";
        std::cout << ss.str() << std::flush;
    }
    
    // Write combined result
    std::string combined = content1 + "\n" + content2 + "\n" + content3;
    auto write_task = async_write_file(std::string("output.txt"), combined);
    int bytes_written = write_task.get();
    
    {
        std::stringstream ss;
        ss << "[process_files] All operations completed\n";
        std::cout << ss.str() << std::flush;
    }
    co_return bytes_written;
}

// Example with error handling
AsyncTask<std::string> safe_read_file(const std::string& filename) {
    try {
        {
            std::stringstream ss;
            ss << "[safe_read_file] Reading: " << filename << "\n";
            std::cout << ss.str() << std::flush;
        }
        
        AsyncRead read_op{filename};
        std::string content = co_await read_op;
        
        co_return content;
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "[safe_read_file] Error: " << e.what() << "\n";
        std::cout << ss.str() << std::flush;
        co_return "ERROR";
    }
}

int main() {
    {
        std::stringstream ss;
        ss << "=== Coroutine-based Async I/O Example ===\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "\n--- Example 1: Single Async Read ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task1 = async_read_file(std::string("config.json"));
    std::string result1 = task1.get();
    {
        std::stringstream ss;
        ss << "Result: " << result1 << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "\n--- Example 2: Single Async Write ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task2 = async_write_file(std::string("log.txt"), std::string("Log entry 1\nLog entry 2\n"));
    int bytes = task2.get();
    {
        std::stringstream ss;
        ss << "Bytes written: " << bytes << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "\n--- Example 3: Multiple Concurrent Operations ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task3 = process_files();
    int total_bytes = task3.get();
    {
        std::stringstream ss;
        ss << "Total bytes processed: " << total_bytes << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "\n--- Example 4: Error Handling ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task4 = safe_read_file(std::string("missing.txt"));
    std::string result4 = task4.get();
    {
        std::stringstream ss;
        ss << "Result: " << result4 << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "\n=== All coroutine examples completed ===\n";
        ss << "\nKey benefits demonstrated:\n";
        ss << "  1. Non-blocking I/O operations\n";
        ss << "  2. Sequential code that runs asynchronously\n";
        ss << "  3. Multiple concurrent operations without threads\n";
        ss << "  4. Clean error handling with try-catch\n";
        ss << "  5. Local variables safely stored in coroutine frame\n";
        std::cout << ss.str() << std::flush;
    }
    
    return 0;
}
