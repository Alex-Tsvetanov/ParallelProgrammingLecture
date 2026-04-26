# Изпитни въпроси: Модерни техники за паралелизъм и асинхронност в C++

## Съдържание
- [Въпроси с избираем отговор](#въпроси-с-избираем-отговор)
- [Отворени въпроси](#отворени-въпроси)
- [Въпроси с код](#въпроси-с-код)

---

## Въпроси с избираем отговор

### 1.
Каква е основната разлика между паралелизъм и асинхронност?

A) Паралелизмът е по-бърз от асинхронността

B) Паралелизмът изпълнява задачи едновременно на множество ядра, докато асинхронността чередува задачи

C) Асинхронността изисква повече памет

D) Няма разлика между двете понятия

**Отговор:** B)

### 2.
Колко пъти по-бързо е използването на Thread Pool в сравнение със създаването на нови нишки за 1000 задачи?

A) 2-5 пъти

B) 10-20 пъти

C) 50-100 пъти

D) 1000 пъти

**Отговор:** C) (Thread Pool ≈ 1-2ms, създаване на нишки ≈ 50-100ms)

### 3.
Кой от следните НЕ е проблем при наивния подход за създаване на нова нишка за всяка задача?

A) System call за създаване

B) Заделяне на stack памет

C) Автоматично балансиране на натоварването

D) Context switching overhead

**Отговор:** C)

### 4.
Какъв е типичният размер на stack паметта за една нишка (thread)?

A) 1-10 KB

B) 100-500 KB

C) 1-2 MB

D) 10-20 MB

**Отговор:** C)

### 5.
Какъв е типичният размер на coroutine frame?

A) По-малко от 1 KB

B) 100 KB - 500 KB

C) 1-2 MB

D) Зависи от операционната система

**Отговор:** A)

### 6.
Коя ключова дума в C++20 се използва за suspend и чакане на резултат в coroutine?

A) `co_return`

B) `co_yield`

C) `co_await`

D) `suspend`

**Отговор:** C)

### 7.
Какво е предимството на lock-free структурите пред lock-based?

A) По-лесна имплементация

B) Non-blocking и по-висока throughput

C) По-малко използване на памет

D) По-добра съвместимост с legacy код

**Отговор:** B)

### 8.
Какво означава CAS в контекста на lock-free програмиране?

A) Cache Allocation System

B) Compare-And-Swap

C) Concurrent Access Synchronization

D) Critical Area Section

**Отговор:** B)

### 9.
Кой memory order гарантира най-силна синхронизация, но е най-бавен?

A) `memory_order_relaxed`

B) `memory_order_acquire`

C) `memory_order_release`

D) `memory_order_seq_cst`

**Отговор:** D)

### 10.
Какво е предназначението на `std::condition_variable` в Thread Pool?

A) Да заключва опашката

B) Да събужда worker нишки когато има нови задачи

C) Да брои задачите

D) Да управлява паметта

**Отговор:** B)

### 11.
В Publisher/Subscriber pattern, какво е ролята на Event Broker?

A) Генерира събития

B) Обработва събития

C) Посредник между publishers и subscribers

D) Съхранява събития в база данни

**Отговор:** C)

### 12.
Коя гаранция за доставка е най-бърза, но може да загуби съобщения?

A) At-least-once

B) Exactly-once

C) At-most-once

D) Guaranteed delivery

**Отговор:** C)

### 13.
Какво е backpressure в контекста на Pub/Sub системи?

A) Натиск върху паметта

B) Publisher генерира събития по-бързо отколкото subscribers могат да обработят

C) Твърде много subscribers

D) Мрежови проблеми

**Отговор:** B)

### 14.
Какво означава RCU в контекста на lock-free структури?

A) Read-Copy-Update

B) Remote Control Unit

C) Recursive Call Utility

D) Reference Counting Unit

**Отговор:** A)

### 15.
Кой тип scheduling използват coroutines?

A) Преемптивен (preemptive)

B) Кооперативен (cooperative)

C) Round-robin

D) Priority-based

**Отговор:** B)

### 16.
Колко е типичното време за context switch при threads?

A) 1-10 наносекунди

B) 10-100 наносекунди

C) 1-10 микросекунди

D) 1-10 милисекунди

**Отговор:** C)

### 17.
Колко е типичното време за context switch при coroutines?

A) 10-100 наносекунди

B) 1-10 микросекунди

C) 100-1000 микросекунди

D) 1-10 милисекунди

**Отговор:** A)

### 18.
Какво е ABA problem в lock-free програмирането?

A) Проблем с алокация на памет

B) Стойност се променя от A на B и обратно на A, което заблуждава CAS операцията

C) Проблем с именуване на променливи

D) Грешка в компилатора

**Отговор:** B)

### 19.
Кой метод на `std::coroutine_handle` се използва за продължаване на изпълнението на coroutine?

A) `continue()`

B) `resume()`

C) `start()`

D) `run()`

**Отговор:** B)

### 20.
Какво връща `std::thread::hardware_concurrency()`?

A) Броя на активните нишки

B) Броя на наличните CPU ядра

C) Максималния брой нишки

D) Текущото натоварване на CPU

**Отговор:** B)

### 21.
Кой от следните НЕ е компонент на Publisher/Subscriber pattern?

A) Publisher

B) Subscriber

C) Event Broker

D) Mutex Manager

**Отговор:** D)

### 22.
Какво е предимството на `compare_exchange_weak` пред `compare_exchange_strong`?

A) По-силна гаранция за успех

B) По-бързо на някои архитектури, но може да fail spuriously

C) По-малко използване на памет

D) По-добра четимост на кода

**Отговор:** B)

### 23.
Къде се съхраняват локалните променливи на coroutine?

A) На stack-а на caller функцията

B) В coroutine frame (heap)

C) В глобална памет

D) В регистрите на CPU

**Отговор:** B)

### 24.
Какво е основното предимство на Pub/Sub pattern пред директната комуникация?

A) По-бързо изпълнение

B) Loose coupling между компоненти

C) По-малко използване на памет

D) По-лесно debugging

**Отговор:** B)

### 25.
Кой memory order се използва за "fire and forget" операции без нужда от синхронизация?

A) `memory_order_seq_cst`

B) `memory_order_acquire`

C) `memory_order_relaxed`

D) `memory_order_release`

**Отговор:** C)

### 26.
Какво е предназначението на `std::suspend_always` в coroutine?

A) Никога не suspend-ва

B) Винаги suspend-ва

C) Suspend-ва при определени условия

D) Унищожава coroutine

**Отговор:** B)

### 27.
Кой от следните е валиден начин за решаване на backpressure?

A) Увеличаване на RAM

B) Throttling на publisher-а

C) Рестартиране на системата

D) Игнориране на проблема

**Отговор:** B)

### 28.
Какво е SPSC Queue?

A) Single Producer Single Consumer Queue

B) Synchronized Parallel Sequential Container

C) Shared Pointer Smart Container

D) System Process Scheduling Controller

**Отговор:** A)

### 29.
Коя библиотека предоставя `tbb::parallel_for`?

A) Boost

B) Qt

C) oneTBB (Threading Building Blocks)

D) OpenMP

**Отговор:** C)

### 30.
Какво е предимството на per-worker queues в Thread Pool?

A) По-лесна имплементация

B) Намалява contention между нишките

C) По-малко използване на памет

D) По-добра съвместимост

**Отговор:** B)

### 31.
Какво е "happens-before" relationship?

A) Времева последователност на събития

B) Гаранция, че операция A е видима за операция B

C) Приоритет на нишки

D) Ред на компилация

**Отговор:** B)

### 32.
Кой метод на `promise_type` се извиква при `co_return value`?

A) `return_void()`

B) `return_value()`

C) `final_suspend()`

D) `get_return_object()`

**Отговор:** B)

### 33.
Какво е cache line alignment и защо е важно?

A) Подравняване на данни за по-бърз достъп и избягване на false sharing

B) Оптимизация на компилатора

C) Метод за криптиране

D) Техника за компресия

**Отговор:** A)

### 34.
Кой от следните е валиден awaitable в C++20?

A) `std::suspend_always`

B) `std::thread`

C) `std::mutex`

D) `std::vector`

**Отговор:** A)

### 35.
Какво е предимството на асинхронната Pub/Sub имплементация с Thread Pool?

A) По-проста имплементация

B) Non-blocking publish() и паралелна обработка

C) По-малко използване на памет

D) По-добра съвместимост с legacy код

**Отговор:** B)

### 36.
Кой метод на awaitable се извиква за да провери дали трябва да suspend-не?

A) `await_suspend()`

B) `await_resume()`

C) `await_ready()`

D) `await_check()`

**Отговор:** C)

### 37.
Кой от следните е правилен начин за capture на променлива в lambda за coroutine?

A) Capture by reference `[&x]`

B) Capture by value `[x]`

C) Capture всичко by reference `[&]`

D) Не се използват lambda в coroutines

**Отговор:** B)

### 38.
Какво е "wait-free" алгоритъм?

A) Алгоритъм без изчакване на I/O

B) Алгоритъм, при който всяка операция завършва в ограничен брой стъпки

C) Алгоритъм без mutex

D) Алгоритъм без threads

**Отговор:** B)

---

## Отворени въпроси

### 39.
Обяснете защо създаването на нова нишка за всяка задача е неефективно и какви проблеми може да причини.

**Отговор:** System call overhead, заделяне на stack памет (1-2 MB), context switching overhead, липса на контрол върху броя нишки.

### 40.
Опишете основните компоненти на Thread Pool и как работят заедно.

**Отговор:** Worker threads, Task queue, Mutex (за опашката), Condition variable (за събуждане).

### 41.
Какво е "contention" при lock-based структури и как lock-free структурите го решават?

**Отговор:** Конкуренция за mutex, водеща до блокиране. Lock-free решава това чрез атомарни операции (CAS) и non-blocking алгоритми.

### 42.
Обяснете разликата между `memory_order_acquire` и `memory_order_release`.

**Отговор:** Acquire се ползва при load (четене) и гарантира видимост след него. Release се ползва при store (запис) и гарантира видимост преди него.

### 43.
Защо coroutines са по-евтини от threads? Посочете поне 4 причини.

**Отговор:** Без system calls, малко състояние (< 1 KB), бърз context switch (ns vs μs), кооперативен scheduling.

### 44.
Какво е "dangling reference" проблемът при coroutines и как се избягва?

**Отговор:** Coroutine държи референция към унищожени данни от stack-а на caller-а. Избягва се чрез capture by value или smart pointers.

### 45.
Опишете трите гаранции за доставка в Pub/Sub системи.

**Отговор:** At-most-once (най-бързо), At-least-once (гарантирано, възможни дубликати), Exactly-once (най-сложно).

### 46.
Какво е Event Loop и каква е ролята му при работа с coroutines?

**Отговор:** Управлява готовите coroutines и извиква `resume()` върху тях; обработва I/O събития.

### 47.
Обяснете какво е "false sharing" и как се избягва.

**Отговор:** Нишки пишат в различни променливи, намиращи се в една cache line. Избягва се чрез alignment/padding.

### 48.
Какви са предимствата и недостатъците на Observer pattern спрямо Publisher/Subscriber pattern?

**Отговор:** Observer е синхронен и тясно свързан. Pub/Sub е асинхронен, loose coupling, но изисква брокер.

### 49.
Опишете как работи Compare-And-Swap (CAS) операцията.

**Отговор:** Атомарно сравнява стойност с очаквана; ако съвпадат, записва нова.

### 50.
Какво е "promise_type" в C++ coroutines и какви методи трябва да съдържа?

**Отговор:** Контролира coroutine поведението: `get_return_object`, `initial_suspend`, `final_suspend`, `unhandled_exception`, `return_value/void`.

### 51.
Обяснете разликата между blocking и non-blocking алгоритми.

**Отговор:** Blocking спира нишката (mutex). Non-blocking ползва atomics и retry (spin), никога не спира нишката напълно.

### 52.
Какво е "ordering" проблемът в Pub/Sub системи и как се решава?

**Отговор:** При паралелна обработка редът може да се наруши. Решения: sequence numbers, partition-based ordering.

### 53.
Какво е "awaitable" в C++20 и какви методи трябва да има?

**Отговор:** Обект за `co_await`. Методи: `await_ready`, `await_suspend`, `await_resume`.

### 54.
Опишете hybrid подхода за комбиниране на Thread Pool, Coroutines и Pub/Sub.

**Отговор:** Pool за CPU задачи, Coroutines за I/O, Pub/Sub за комуникация.

### 55.
Какво е "coroutine frame" и какво съдържа?

**Отговор:** Heap структура с promise object, локални променливи, resume point.

### 56.
Какво е "priority inversion"?

**Отговор:** Нископриоритетна нишка държи lock, блокирайки високоприоритетна.

### 57.
Какви са приложенията на Publisher/Subscriber pattern в реални системи?

**Отговор:** GUI, IoT, Microservices, Game Engines.

### 58.
Обяснете разликата между `co_await`, `co_yield` и `co_return`.

**Отговор:** `co_await` (suspend/wait), `co_yield` (suspend/emit value), `co_return` (finish).

### 59.
Какво е "thread-safe" и какви техники се използват за постигането му?

**Отговор:** Код без race conditions. Техники: Mutex, atomics, immutable data.

### 60.
Какво е oneTBB и какви паралелни алгоритми предоставя?

**Отговор:** Intel библиотека: `parallel_for`, `parallel_reduce` и др.

### 61.
Какво е "memory barrier" и защо е необходим?

**Отговор:** Инструкция за предотвратяване на CPU/Compiler reordering на паметта.

### 62.
Обяснете разликата между синхронна и асинхронна Pub/Sub имплементация.

**Отговор:** Синхронна: блокира publish до обработка. Асинхронна: publish връща веднага, pool обработва.

### 63.
Какво е "idempotent" операция и защо е важна при at-least-once доставка?

**Отговор:** Операция, която може да се повтаря без странични ефекти. Важна при дублиране на съобщения.

### 64.
Какво е "bounded queue" и защо е важна при backpressure?

**Отговор:** Опашка с лимит. Предпазва от препълване на паметта.

### 65.
Обяснете какво е "cache locality" и защо Thread Pool подобрява cache locality.

**Отговор:** Достъп до близка памет. Pool преизползва нишки и данни, запазвайки ги в кеша.

---

## Въпроси с код

### 66.
Напишете проста имплементация на Thread Pool с 2 worker нишки.

**Отговор:**

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t threads = 2) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(queue_mutex); stop = true; }
        condition.notify_all();
        for (auto& w : workers) w.join();
    }
};
```

### 67.
Напишете lock-free counter използвайки `std::atomic` и `compare_exchange_weak`.

**Отговор:**
```cpp
#include <atomic>

class LockFreeCounter {
    std::atomic<int> count{0};

public:
    void increment() {
        int expected = count.load(std::memory_order_relaxed);
        while (!count.compare_exchange_weak(expected, expected + 1, std::memory_order_release, std::memory_order_relaxed)) {}
    }

    int get() const {
        return count.load(std::memory_order_acquire);
    }
};
```

### 68.
Напишете проста coroutine, която връща сумата на две числа.

**Отговор:**

```cpp
#include <coroutine>

template<typename T>
struct Task {
    struct promise_type {
        T value;
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = v; }
        void unhandled_exception() { std::terminate(); }
    };
    std::coroutine_handle<promise_type> handle;
    T get() { return handle.promise().value; }
    ~Task() { if (handle) handle.destroy(); }
};

Task<int> add_async(int a, int b) { co_return a + b; }
```

### 69.
Напишете прост EventBroker за Publisher/Subscriber pattern.

**Отговор:**

```cpp
#include <functional>
#include <vector>

template<typename Event>
class EventBroker {
    std::vector<std::function<void(const Event&)>> subscribers;

public:
    void subscribe(std::function<void(const Event&)> cb) {
        subscribers.push_back(std::move(cb));
    }

    void publish(const Event& event) {
        for (const auto& sub : subscribers) sub(event);
    }
};
```

### 70.
Напишете custom awaitable, който suspend-ва за поне определен брой милисекунди (или симулира Async IO).

**Отговор:**

```cpp
// Awaitable за симулиране на async I/O
struct AsyncIO {
    std::string filename;
    int delay_ms;
    bool await_ready() { return false; }  // Винаги suspend
    void await_suspend(std::coroutine_handle<> h) {
        // Simulate async operation
        std::thread([h, delay = delay_ms]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            EventLoop::instance().schedule(h);  // Resume след завършване
        }).detach();
    }
    std::string await_resume() {
        return "Content from " + filename;
    }
};
AsyncTask<std::string> read_file_async(const std::string& file) {
    std::cout << "Starting read: " << file << "\n";
    auto content = co_await AsyncIO{file, 100};  // Suspend тук!
    std::cout << "Completed read: " << file << "\n";
    co_return content;
}
```

### 71.
Напишете producer-consumer пример с `std::condition_variable`.

**Отговор:**

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;
bool done = false;

void producer(int count) {
    for (int i = 0; i < count; ++i) {
        { std::lock_guard<std::mutex> lock(mtx); buffer.push(i); }
        cv.notify_one();
    }
    { std::lock_guard<std::mutex> lock(mtx); done = true; }
    cv.notify_all();
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || done; });
        while (!buffer.empty()) { buffer.pop(); }
        if (done && buffer.empty()) break;
    }
}
```

### 72.
Напишете acquire-release синхронизация между producer и consumer.

**Отговор:**

```cpp
#include <atomic>
#include <thread>

std::atomic<int> data{0};
std::atomic<bool> ready{false};

void producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_release);
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)) {}
    int value = data.load(std::memory_order_relaxed); // Гарантирано 42
}
```

### 73.
Напишете RAII wrapper за `std::mutex` (или използвайте `std::lock_guard` в пример).

**Отговор:**

```cpp
#include <mutex>

template<typename Mutex>
class MyLockGuard {
    Mutex& mutex_;
public:
    explicit MyLockGuard(Mutex& m) : mutex_(m) { mutex_.lock(); }
    ~MyLockGuard() { mutex_.unlock(); }
    MyLockGuard(const MyLockGuard&) = delete;
    MyLockGuard& operator=(const MyLockGuard&) = delete;
};
```

### 74.
Напишете паралелен for цикъл използвайки threads (или `tbb::parallel_for` синтаксис).

**Отговор:**

```cpp
#include <thread>
#include <vector>

template<typename Func>
void parallel_for(int start, int end, int num_threads, Func f) {
    std::vector<std::thread> threads;
    int chunk = (end - start) / num_threads;
    
    for (int t = 0; t < num_threads; ++t) {
        int begin = start + t * chunk;
        int finish = (t == num_threads - 1) ? end : begin + chunk;
        threads.emplace_back([=]() {
            for (int i = begin; i < finish; ++i) f(i);
        });
    }
    for (auto& th : threads) th.join();
}
```

### 75.
Напишете bounded queue с фиксиран капацитет (SPSC вариант или с mutex).

**Отговор:**

```cpp
#include <mutex>

template<typename Mutex>
class MyLockGuard {
    Mutex& mutex_;
public:
    explicit MyLockGuard(Mutex& m) : mutex_(m) { mutex_.lock(); }
    ~MyLockGuard() { mutex_.unlock(); }
    MyLockGuard(const MyLockGuard&) = delete;
    MyLockGuard& operator=(const MyLockGuard&) = delete;
};
```

### 76.
Напишете функция за паралелно сумиране на масив (reduce).

**Отговор:**

```cpp
#include <thread>
#include <vector>
#include <numeric>

long long parallel_sum(const std::vector<int>& data, int num_threads) {
    std::vector<long long> partial_sums(num_threads, 0);
    std::vector<std::thread> threads;
    size_t chunk = data.size() / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t == num_threads - 1) ? data.size() : start + chunk;
        threads.emplace_back([&, t, start, end]() {
            partial_sums[t] = std::accumulate(data.begin() + start, data.begin() + end, 0LL);
        });
    }
    for (auto& th : threads) th.join();
    return std::accumulate(partial_sums.begin(), partial_sums.end(), 0LL);
}
```

### 77.
Напишете thread-safe counter с mutex.

**Отговор:**

```cpp
#include <mutex>

class ThreadSafeCounter {
    int count_ = 0;
    mutable std::mutex mutex_;

public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
    }

    void decrement() {
        std::lock_guard<std::mutex> lock(mutex_);
        --count_;
    }

    int get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
};
```

### 78.
Напишете async EventBroker с Thread Pool.

**Отговор:**

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t threads = 2) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {
        { std::lock_guard<std::mutex> lock(queue_mutex); tasks.push(std::move(task)); }
        condition.notify_one();
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(queue_mutex); stop = true; }
        condition.notify_all();
        for (auto& w : workers) w.join();
    }
};

template<typename Event>
class AsyncEventBroker {
    std::vector<std::function<void(const Event&)>> subscribers;
    ThreadPool& pool;
    std::mutex mtx;

public:
    AsyncEventBroker(ThreadPool& p) : pool(p) {}

    void subscribe(std::function<void(const Event&)> cb) {
        std::lock_guard<std::mutex> lock(mtx);
        subscribers.push_back(std::move(cb));
    }

    void publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& sub : subscribers) {
            pool.enqueue([sub, event]() { sub(event); });
        }
    }
};
```

### 79.
Напишете coroutine Task с exception handling (`unhandled_exception`).

**Отговор:**

```cpp
#include <coroutine>
#include <exception>

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
        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    T get() {
        if (handle.promise().exception)
            std::rethrow_exception(handle.promise().exception);
        return handle.promise().value;
    }

    ~Task() { if (handle) handle.destroy(); }
};
```

### 80.
Напишете SPSC (Single Producer Single Consumer) lock-free queue.

**Отговор:**

```cpp
#include <atomic>
#include <array>

template<typename T, size_t Size = 1024>
class SPSCQueue {
    std::array<T, Size> buffer;
    std::atomic<size_t> head{0};
    std::atomic<size_t> tail{0};

public:
    bool enqueue(const T& item) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t next = (t + 1) % Size;
        if (next == head.load(std::memory_order_acquire)) return false;
        buffer[t] = item;
        tail.store(next, std::memory_order_release);
        return true;
    }

    bool dequeue(T& item) {
        size_t h = head.load(std::memory_order_relaxed);
        if (h == tail.load(std::memory_order_acquire)) return false;
        item = buffer[h];
        head.store((h + 1) % Size, std::memory_order_release);
        return true;
    }
};
```
### 81.
Напишете функция за паралелно търсене в масив.

**Отговор:**

```cpp
#include <thread>
#include <vector>
#include <atomic>

template<typename T>
int parallel_find(const std::vector<T>& data, const T& target, int num_threads) {
    std::atomic<int> result{-1};
    std::vector<std::thread> threads;
    size_t chunk = data.size() / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t == num_threads - 1) ? data.size() : start + chunk;
        threads.emplace_back([&, start, end]() {
            for (size_t i = start; i < end && result.load() == -1; ++i) {
                if (data[i] == target) {
                    result.store(static_cast<int>(i));
                    return;
                }
            }
        });
    }
    for (auto& th : threads) th.join();
    return result.load();
}
```

### 82.
Напишете coroutine scheduler (event loop).

**Отговор:**

```cpp
#include <coroutine>
#include <queue>
#include <mutex>

class Scheduler {
    std::queue<std::coroutine_handle<>> ready_queue;
    std::mutex mtx;

public:
    void schedule(std::coroutine_handle<> handle) {
        std::lock_guard<std::mutex> lock(mtx);
        ready_queue.push(handle);
    }

    void run() {
        while (true) {
            std::coroutine_handle<> handle;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (ready_queue.empty()) break;
                handle = ready_queue.front();
                ready_queue.pop();
            }
            if (!handle.done()) handle.resume();
        }
    }
};
```

### 83.
Напишете функция за map-reduce паралелна обработка.

**Отговор:**

```cpp
#include <thread>
#include <vector>
#include <functional>

template<typename T, typename MapFunc, typename ReduceFunc>
T parallel_map_reduce(const std::vector<T>& data, MapFunc map_fn, 
                      ReduceFunc reduce_fn, T initial, int num_threads) {
    std::vector<T> partial_results(num_threads);
    std::vector<std::thread> threads;
    size_t chunk = data.size() / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t == num_threads - 1) ? data.size() : start + chunk;
        threads.emplace_back([&, t, start, end]() {
            T result = initial;
            for (size_t i = start; i < end; ++i)
                result = reduce_fn(result, map_fn(data[i]));
            partial_results[t] = result;
        });
    }
    for (auto& th : threads) th.join();

    T final_result = initial;
    for (const auto& r : partial_results)
        final_result = reduce_fn(final_result, r);
    return final_result;
}
```

### 84.
Напишете RCU-style lock-free subscriber list.

**Отговор:**

```cpp
#include <atomic>
#include <memory>
#include <functional>

template<typename Event>
class RCUEventBroker {
    struct Node {
        std::function<void(const Event&)> callback;
        std::shared_ptr<Node> next;
        Node(std::function<void(const Event&)> cb) : callback(std::move(cb)) {}
    };
    std::shared_ptr<Node> head;

public:
    void subscribe(std::function<void(const Event&)> cb) {
        auto new_node = std::make_shared<Node>(std::move(cb));
        auto old_head = std::atomic_load(&head);
        do {
            new_node->next = old_head;
        } while (!std::atomic_compare_exchange_weak(&head, &old_head, new_node));
    }

    void publish(const Event& event) {
        auto node = std::atomic_load(&head);
        while (node) {
            node->callback(event);
            node = node->next;
        }
    }
};
```

### 85.
Напишете hybrid система комбинираща Thread Pool и Pub/Sub.

**Отговор:**

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t threads = 2) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {
        { std::lock_guard<std::mutex> lock(queue_mutex); tasks.push(std::move(task)); }
        condition.notify_one();
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(queue_mutex); stop = true; }
        condition.notify_all();
        for (auto& w : workers) w.join();
    }
};

struct Task { int id; std::string data; };

template<typename Event>
class HybridSystem {
    std::vector<std::function<void(const Event&)>> subscribers;
    ThreadPool pool;
    std::mutex mtx;

public:
    HybridSystem(size_t threads = 4) : pool(threads) {}

    void subscribe(std::function<void(const Event&)> cb) {
        std::lock_guard<std::mutex> lock(mtx);
        subscribers.push_back(std::move(cb));
    }

    void publish(const Event& event) {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& sub : subscribers) {
            pool.enqueue([sub, event]() { sub(event); });
        }
    }
};

// Използване:
// HybridSystem<Task> system(4);
// system.subscribe([](const Task& t) { process(t); });
// system.publish({1, "data"});
```