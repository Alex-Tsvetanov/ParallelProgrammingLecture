// Example 3: Basic Coroutine Example
// Demonstrates C++20 coroutines with co_await, co_return
// Topics: std::coroutine_handle, promise_type, suspend_always

#include <coroutine>
#include <iostream>
#include <string>
#include <sstream>

// Simple Task coroutine type
template<typename T>
struct Task {
    struct promise_type {
        T value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T v) {
            value = std::move(v);
        }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    std::coroutine_handle<promise_type> handle;

    Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task() {
        if (handle) handle.destroy();
    }

    // Move-only
    Task(Task&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = std::exchange(other.handle, nullptr);
        }
        return *this;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    T get() {
        if (!handle.done()) {
            handle.resume();
        }
        if (handle.promise().exception) {
            std::rethrow_exception(handle.promise().exception);
        }
        return handle.promise().value;
    }
};

// Simple awaitable that always suspends
struct Suspend {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<>) const noexcept {
        std::stringstream ss;
        ss << "  [Suspended]\n";
        std::cout << ss.str() << std::flush;
    }
    void await_resume() const noexcept {
        std::stringstream ss;
        ss << "  [Resumed]\n";
        std::cout << ss.str() << std::flush;
    }
};

// Example coroutine function
Task<int> compute_async(int a, int b) {
    {
        std::stringstream ss;
        ss << "Starting computation with " << a << " and " << b << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    co_await Suspend{};
    {
        std::stringstream ss;
        ss << "After first suspension\n";
        std::cout << ss.str() << std::flush;
    }
    
    int intermediate = a + b;
    {
        std::stringstream ss;
        ss << "Intermediate result: " << intermediate << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    co_await Suspend{};
    {
        std::stringstream ss;
        ss << "After second suspension\n";
        std::cout << ss.str() << std::flush;
    }
    
    int result = intermediate * 2;
    {
        std::stringstream ss;
        ss << "Final computation complete\n";
        std::cout << ss.str() << std::flush;
    }
    
    co_return result;
}

// Coroutine that demonstrates local variable safety
Task<std::string> string_coroutine() {
    std::string local_data = "This is SAFE - stored in coroutine frame";
    {
        std::stringstream ss;
        ss << "Created local string: " << local_data << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    co_await Suspend{};
    
    // local_data is still valid here because it's in the coroutine frame
    {
        std::stringstream ss;
        ss << "After suspension, local_data is still: " << local_data << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    co_return local_data + " (returned)";
}

int main() {
    {
        std::stringstream ss;
        ss << "=== Basic Coroutine Example ===\n\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "--- Example 1: Compute Async ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task1 = compute_async(5, 10);
    {
        std::stringstream ss;
        ss << "Task created, coroutine started\n";
        std::cout << ss.str() << std::flush;
    }
    int result = task1.get();
    {
        std::stringstream ss;
        ss << "Result: " << result << "\n\n";
        std::cout << ss.str() << std::flush;
    }
    
    {
        std::stringstream ss;
        ss << "--- Example 2: String Coroutine (Local Variable Safety) ---\n";
        std::cout << ss.str() << std::flush;
    }
    auto task2 = string_coroutine();
    {
        std::stringstream ss;
        ss << "Task created\n";
        std::cout << ss.str() << std::flush;
    }
    std::string str_result = task2.get();
    {
        std::stringstream ss;
        ss << "Result: " << str_result << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    return 0;
}
