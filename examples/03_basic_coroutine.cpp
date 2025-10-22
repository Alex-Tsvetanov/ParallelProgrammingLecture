// Example 3: Basic Coroutine Example
// Demonstrates C++20 coroutines with co_await, co_return
// Topics: std::coroutine_handle, promise_type, suspend_always

#include <coroutine>
#include <iostream>
#include <string>

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
        std::cout << "  [Suspended]\n";
    }
    void await_resume() const noexcept {
        std::cout << "  [Resumed]\n";
    }
};

// Example coroutine function
Task<int> compute_async(int a, int b) {
    std::cout << "Starting computation with " << a << " and " << b << "\n";
    
    co_await Suspend{};
    std::cout << "After first suspension\n";
    
    int intermediate = a + b;
    std::cout << "Intermediate result: " << intermediate << "\n";
    
    co_await Suspend{};
    std::cout << "After second suspension\n";
    
    int result = intermediate * 2;
    std::cout << "Final computation complete\n";
    
    co_return result;
}

// Coroutine that demonstrates local variable safety
Task<std::string> string_coroutine() {
    std::string local_data = "This is SAFE - stored in coroutine frame";
    std::cout << "Created local string: " << local_data << "\n";
    
    co_await Suspend{};
    
    // local_data is still valid here because it's in the coroutine frame
    std::cout << "After suspension, local_data is still: " << local_data << "\n";
    
    co_return local_data + " (returned)";
}

int main() {
    std::cout << "=== Basic Coroutine Example ===\n\n";
    
    std::cout << "--- Example 1: Compute Async ---\n";
    auto task1 = compute_async(5, 10);
    std::cout << "Task created, coroutine started\n";
    int result = task1.get();
    std::cout << "Result: " << result << "\n\n";
    
    std::cout << "--- Example 2: String Coroutine (Local Variable Safety) ---\n";
    auto task2 = string_coroutine();
    std::cout << "Task created\n";
    std::string str_result = task2.get();
    std::cout << "Result: " << str_result << "\n";
    
    return 0;
}
