# Банка изпитни въпроси (AI-resistant): паралелизъм, асинхронност и конкурентност в C++

## Как да се ползва при оценяване
- Всеки въпрос е **самодостатъчен**. Ако е нужен код, той е включен като snippet.
- При свободните въпроси търсете **кратка, конкретна обосновка** (1-3 ясни причини) и/или конкретен interleaving, а не общи дефиниции.
- За всеки въпрос има секция „Какво вероятно ще отговори стандартен LLM“, която съдържа **типични общи/учебникарски отговори** (червени флагове):
  - липсва конкретика за дадения snippet,
  - пропуска ключов детайл/число/условие,
  - звучи правдоподобно, но не отговаря на конкретния казус.

---

## Въпроси

### 1) (Свободен теоретичен) Thread Pool: кога и защо `enqueue()` може да хвърли изключение?
Фрагмент:
```cpp
class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;

public:
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
```

**Въпрос:** Посочете **конкретен сценарий** (коя нишка какво прави), при който `enqueue()` хвърля изключение. Обяснете защо това е правилно поведение и кога би било грешно (ако изобщо).

**Верен отговор / ключови думи:**
- **Сценарий:** едната нишка влиза в деструктора и set-ва `stop=true` под `queue_mutex`, докато друга нишка (външен producer) се опитва да `enqueue` и след като вземе `queue_mutex` вижда `stop==true`.
- **Защо е правилно:** предотвратява добавяне на задачи в pool, който вече няма гаранция, че ще ги изпълни (или ще е в процес на shutdown).
- **Критичен детайл:** трябва да се обсъди кой е „owner“ на pool-а; ако външен код може да вика `enqueue` конкурентно със destruction, това е **API договор** проблем.
- **Допълнение:** алтернатива е `enqueue` да връща `bool` вместо exception (design choice).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Хвърля изключение, ако пулът е спрян, за да се избегнат race conditions.“ (твърде общо; липсва конкретен интерлевинг и API договор)

---

### 2) (Свободен теоретичен) Thread Pool: възможно ли е worker да „заспи завинаги“?
Фрагмент:
```cpp
std::mutex queue_mutex;
std::condition_variable condition;
std::queue<int> tasks;
bool stop = false;

std::unique_lock<std::mutex> lock(queue_mutex);
condition.wait(lock, [&] {
    return stop || !tasks.empty();
});

if (stop && tasks.empty()) {
    return;
}
```

**Въпрос:** Ако махнете predicate-а и напишете само `condition.wait(lock);`, може ли програмата да блокира безкрайно или да работи грешно? Посочете **една конкретна** причина и **един** възможен ефект.

**Верен отговор / ключови думи:**
- **Spurious wakeups**: `wait` може да се събуди без `notify`.
- Без predicate worker може:
  - да продължи с празна опашка (и да dereference-не `front()`/`pop()` неправилно), или
  - да пропусне shutdown сигнал/условие (ако се разчита на точен момент на `notify`), или
  - да прави busy-loop около `wait`/проверки.
- В този код predicate-а гарантира правилност при spurious wakeups и при състезание между `notify` и `wait`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Predicate се използва за ефективност.“ (не е само ефективност; ключът е коректност при spurious wakeups)

---

### 3) (Свободен теоретичен) Thread Pool: кои редове могат да се появят в „неочакван“ ред и защо?
Фрагмент:
```cpp
std::cout << "\nAll tasks enqueued. Waiting for completion...\n";
std::this_thread::sleep_for(std::chrono::seconds(2));
std::cout << "\nMain thread exiting (pool destructor will wait for workers)\n";
```

**Въпрос:** Възможно ли е да видите `Main thread exiting ...` **преди** някои `Task X completed`? Ако да, защо това не е логическа грешка? Ако не, при какво допълнително условие би било невъзможно?

**Верен отговор / ключови думи:**
- Да, по принцип е възможно main да принтне съобщението преди някои задачи да са приключили, защото:
  - `sleep_for(2s)` е само „вероятностен“ wait, не е синхронизация,
  - задачите се изпълняват в други нишки и stdout е interleaved.
- Не е логическа грешка, защото реалната гаранция е в деструктора: `join()` чака worker-ите.
- **Невъзможно** би било само ако има изрична синхронизация (напр. `wait_all()`/barrier) преди принта.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Не, деструкторът чака всички задачи, значи винаги първо ще завършат.“ (смесва „преди края на процеса“ с „преди конкретен print“)

---

### 4) (Свободен теоретичен) Lock-free queue: има ли use-after-free риск при повече от 1 consumer?
Фрагмент:
```cpp
using T = int;

struct Node {
    std::shared_ptr<T> data;
    std::atomic<Node*> next{nullptr};
};

std::atomic<Node*> head;

std::shared_ptr<T> dequeue() {
    Node* old_head = head.load(std::memory_order_acquire);

    while (true) {
        Node* next = old_head->next.load(std::memory_order_acquire);
        if (next == nullptr) {
            return nullptr;
        }

        if (head.compare_exchange_weak(
            old_head, next,
            std::memory_order_release,
            std::memory_order_acquire)) {
            std::shared_ptr<T> result = next->data;
            delete old_head;
            return result;
        }
    }
}
```

**Въпрос:** В показания код има **2 consumer нишки**. Дали `delete old_head;` е безопасно при MPMC (multiple consumers)? Отговорете с **ДА/НЕ** и обосновете с 2-3 изречения, като опишете възможен interleaving.

**Верен отговор / ключови думи:**
- **НЕ** (по общия случай за MPMC).
- Възможен сценарий:
  - Consumer A и B прочитат `old_head` (същия адрес).
  - A печели CAS и `delete`-ва `old_head`.
  - B (или друг consumer) може да dereference-не вече освободения `old_head` още при следващо четене от него (напр. `old_head->next.load(...)`), ако освобождаването се случи преди B да е приключил с използването на този pointer.
- Нужни техники: hazard pointers / epoch reclamation / reference counting върху node-ове.
- Забележка: това е „учебен“ пример; реален lock-free MPMC queue е по-сложен.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Да, защото всичко е атомарно.“ (атомарност на pointers ≠ безопасна reclamation)

---

### 5) (Избираем отговор) Lock-free queue: винаги ли ще изведе `Queue empty: yes`?
Фрагмент:
```cpp
const int num_producers = 3;
const int num_consumers = 2;
const int items_per_producer = 5;

int items_per_consumer = (num_producers * items_per_producer) / num_consumers;
// ... стартира consumers с items_per_consumer
```

**Въпрос:** Изчислете точно колко е `items_per_consumer`. След това отговорете: възможно ли е след `join()` да има останали елементи в опашката? Ако да, колко (минимум)?

**Верен отговор / ключови думи:**
- Общо produced: `3 * 5 = 15`.
- `items_per_consumer = 15 / 2 = 7` (целочислено).
- Общо consumed: `2 * 7 = 14`.
- Остава поне `1` елемент в опашката (ако няма друг механизъм да се консумира).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Не, след join опашката е празна.“ (пропуска целочисленото деление)

---

### 6) (Свободен теоретичен) Acquire/Release: защо `data.load(relaxed)` е OK след `ready.load(acquire)`?
Фрагмент:
```cpp
std::atomic<int> data{0};
std::atomic<bool> ready{false};

// producer
data.store(100, std::memory_order_relaxed);
ready.store(true, std::memory_order_release);

// consumer
while (!ready.load(std::memory_order_acquire)) {}
int value = data.load(std::memory_order_relaxed);
```

**Въпрос:** Обяснете защо `data.load(relaxed)` все пак е коректно (защо вижда 100), при условие че `data.store` е `relaxed`.

**Верен отговор / ключови думи:**
- `ready.store(release)` + `ready.load(acquire)` създава **synchronizes-with** и следователно **happens-before**.
- Всички записи **преди** release в producer (вкл. `data.store(100, relaxed)`) стават видими **след** acquire в consumer.
- След като `ready` е видяно като `true` с `acquire`, четенето на `data` може да е `relaxed`, защото редът/видимостта вече са гарантирани от release-acquire на `ready`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Acquire и release гарантират синхронизация.“ (твърде общо; липсва happens-before логика „преди release / след acquire“)

---

### 7) (Свободен теоретичен) `compare_exchange_weak`: какво е „spurious failure“ и защо кодът е в `while`?
Фрагмент:
```cpp
std::atomic<int> count{0};

int expected = count.load(std::memory_order_relaxed);
while (!count.compare_exchange_weak(
    expected,
    expected + 1,
    std::memory_order_release,
    std::memory_order_relaxed)) {
}
```

**Въпрос:** Дайте обяснение с 2 части:
1) Какво означава „spurious failure“ при `compare_exchange_weak`.
2) Защо параметърът `expected` е променлива (и защо се подава по референция от API-то).

**Верен отговор / ключови думи:**
- `weak` може да върне `false` **дори когато стойността съвпада** (специфика + хардуерна оптимизация), затова се retry-ва.
- При failure `compare_exchange_*` обновява `expected` с текущата стойност на atomica, за да може следващият retry да сравнява с актуалното.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „weak е по-бърз, затова се използва цикъл.“ (не стига; липсва ключът, че `expected` се обновява)

---

### 8) (Свободен теоретичен) Spinlock: какво гарантират `acquire` и `release` тук?
Фрагмент:
```cpp
std::atomic_flag flag = ATOMIC_FLAG_INIT;

while (flag.test_and_set(std::memory_order_acquire)) {
    std::this_thread::yield();
}
// critical section
flag.clear(std::memory_order_release);
```

**Въпрос:** Ако смените `acquire/release` с `relaxed/relaxed`, какъв тип проблем може да се появи? Дайте **пример** със „споделена променлива“, която се обновява в критичната секция.

**Верен отговор / ключови думи:**
- Без acquire/release може да няма правилна **видимост** на записи в критичната секция между нишките.
- Пример: `shared_data++` в критична секция; друга нишка може да „види“ lock-а освободен, но да не види обновената стойност на `shared_data` (нарушена happens-before връзка между unlock и следващ lock).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „relaxed може да доведе до race condition.“ (твърде общо; spinlock пак „заключва“, но губи гаранции за ordering/visibility)

---

### 9) (Свободен теоретичен) Double-checked locking: кои двата memory-order детайла са критични?
Фрагмент:
```cpp
struct Singleton {
    int x = 0;
};

std::atomic<Singleton*> instance{nullptr};
std::mutex mutex;

Singleton* tmp = instance.load(std::memory_order_acquire);
if (tmp == nullptr) {
    std::lock_guard<std::mutex> lock(mutex);
    tmp = instance.load(std::memory_order_relaxed);
    if (tmp == nullptr) {
        tmp = new Singleton();
        instance.store(tmp, std::memory_order_release);
    }
}
```

**Въпрос:** Посочете **две** неща, които трябва да са верни, за да е коректно това (в рамките на показания код), и **едно** нещо, което остава проблем (дизайнерски/ресурсен).

**Верен отговор / ключови думи:**
- Коректност:
  - Първото четене трябва да е `acquire`, за да вижда напълно конструирания обект след `release` store.
  - `store` трябва да е `release`, за да не се „размести“ публикуването на pointer-а преди конструирането.
- Проблем:
  - няма освобождаване на `new Singleton()` (leak), няма lifetime management; или „order of destruction“ при shutdown.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Mutex решава проблема.“ (mutex е само за slow path; критични са acquire/release при публикацията)

---

### 10) (Поправяне на грешки) Coroutine: намерете логическа грешка в `Task<T>::get()`
Фрагмент:
```cpp
// Минимален awaitable, който винаги suspend-ва.
struct Suspend {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};

template<typename T>
struct Task {
    struct promise_type {
        T value;                 // няма инициализация
        std::exception_ptr ex;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = v; }
        void unhandled_exception() { ex = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> h;

    T get() {
        if (!h.done()) {
            h.resume();
        }
        if (h.promise().ex) {
            std::rethrow_exception(h.promise().ex);
        }
        return h.promise().value;
    }
};

Task<int> compute_async(int a, int b) {
    co_await Suspend{};
    int intermediate = a + b;
    co_await Suspend{};
    co_return intermediate * 2;
}
```

**Въпрос:** Обяснете защо `Task<int> t = compute_async(5, 10); int r = t.get();` е логически грешно с този `get()`. Какъв може да е ефектът при първото извикване на `get()`?

**Верен отговор / ключови думи:**
- `resume()` се извиква **само веднъж**.
- `compute_async` се suspend-ва втори път преди `co_return`, т.е. coroutine не е `done()` след едно `resume()`.
- Връщането на `promise().value` става преди `return_value`, т.е. връща **неинициализирана/стара стойност** (логическа грешка/UB).
- Корекция: `get()` трябва да „драйв“-ва coroutine до completion (loop до `done()`), или да има scheduler/event loop.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Трябва да се добави повече синхронизация.“ (проблемът е control-flow на coroutine, не threading)

---

### 11) (Свободен теоретичен) Coroutine lifetime: защо `local_data` е безопасен след `co_await`?
Фрагмент:
```cpp
// Минимален awaitable, който винаги suspend-ва.
struct Suspend {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};

// Приемете, че Task<int> е валиден coroutine return type.
Task<int> compute_lifetime_demo() {
    std::string local_data = "abc";
    co_await Suspend{};
    co_return static_cast<int>(local_data.size());
}
```

**Въпрос:** Обяснете защо `local_data` остава валиден след `co_await` (какво се случва с lifetime-а му). Дайте и **един** пример за подобен код, който би бил опасен (dangling reference/pointer) в coroutine контекст.

**Верен отговор / ключови думи:**
- Локалните променливи, които „преживяват“ suspension point, се съхраняват в **coroutine frame** (не в stack-а на caller-а).
- Те остават валидни до разрушаване на coroutine-а (след `final_suspend` и destroy на handle-а).
- Опасен пример: връщане/запазване на pointer/ref към обект, който не е в coroutine frame (напр. референция към локална променлива на caller-а, capture-by-reference в async callback), или използване на `local_data.c_str()` след като coroutine вече е унищожен.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Coroutine е като thread, затова локалните променливи са винаги safe.“ (липсва конкретика за coroutine frame и кога точно става dangling)

---

### 12) (Свободен теоретичен) Async I/O awaitable: защо тук `filename` се копира?
Фрагмент:
```cpp
std::string fname = filename;
std::thread([fname, result_ptr, handle, this]() mutable {
    std::this_thread::sleep_for(...);
    *result_ptr = "Contents of " + fname;
    const_cast<AsyncRead*>(this)->result = *result_ptr;
    EventLoop::instance().schedule(handle);
}).detach();
```

**Въпрос:** Защо е важно `fname` да е копие (capture by value) в background thread-а? Какъв конкретен bug може да се случи, ако вместо това се използва референция към `filename`?

**Верен отговор / ключови думи:**
- Background thread живее по-дълго от текущия stack-frame на `await_suspend` и потенциално по-дълго от caller контекст.
- Ако се използва референция, може да се получи **dangling reference** към string, който вече е унищожен/преместен.
- Копието фиксира lifetime-а независимо от външния контекст.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Копирането е по-безопасно.“ (липсва конкретното lifetime обяснение)

---

### 13) (Свободен теоретичен) EventLoop: защо `run()` е написан да спира при празна опашка?
Фрагмент:
```cpp
class EventLoop {
    std::queue<std::coroutine_handle<>> ready_queue;
    std::mutex queue_mutex;

public:
    void schedule(std::coroutine_handle<> h) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        ready_queue.push(h);
    }

    void run() {
        while (true) {
            std::coroutine_handle<> h;
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (ready_queue.empty()) {
                    break;
                }
                h = ready_queue.front();
                ready_queue.pop();
            }
            if (!h.done()) {
                h.resume();
            }
        }
    }
};

// Приемете, че get() периодично вика loop.run(), докато даден handle стане done().
```

**Въпрос:** Ако `run()` беше „вечен цикъл“ (никога не `break`-ва), какъв проблем би имало за `AsyncTask<T>::get()`? И обратно: как `get()` компенсира това, че `run()` може да свърши преди да пристигне следващото I/O събитие?

**Верен отговор / ключови думи:**
- Вечен `run()` може да блокира main thread без възможност за излизане (и да гори CPU при busy-wait).
- `get()` прави loop: `run()` + кратък `sleep_for(10ms)` докато `handle.done()` стане true.
- Така се „пулинг“-ва до пристигане на нови handles, без `run()` да трябва да е блокиращ.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Event loop обработва задачи.“ (твърде общо; търси се взаимодействието между `run()` и `get()`)

---

### 14) (Поправяне на грешки) Async Pub/Sub: защо lambda capture-ва `subscriber` по стойност?
Фрагмент:
```cpp
for (const auto& subscriber : subscribers) {
    pool.enqueue([subscriber, event]() {
        subscriber(event);
    });
}
```

**Въпрос:** Какъв конкретен проблем би се появил, ако lambda capture-ва `subscriber` по референция (`[&subscriber, &event]`)? Отговорете с 2 изречения.

**Верен отговор / ключови думи:**
- `subscriber` е reference към елемент от vector в цикъла; след итерацията референцията в lambda ще сочи към **променяща се** променлива/следващ елемент (или вече невалиден).
- `event` е параметър на `publish`; при асинхронно изпълнение може да е **излязъл от scope** -> dangling reference.
- Capture by value прави snapshot.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „За thread-safety.“ (проблемът е lifetime на referenc-и)

---

### 15) (Свободен теоретичен) Async Pub/Sub: какво означава „publish() е non-blocking“ в този пример?
**Въпрос:** В този код `publish()` взима mutex, enqueue-ва задачи и връща. В какъв смисъл е „non-blocking“ и в какъв смисъл **не е** (все пак блокира)? Дайте по една точка за всяко.

**Верен отговор / ключови думи:**
- „Non-blocking“: не чака subscriber-ите да обработят събитието (няма `join`/barrier); връща преди обработката да е завършила.
- „Все пак блокира“: взима `mutex` върху `subscribers`, може да чака други publishers/subscribers (contended lock).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Non-blocking означава без mutex.“ (не; може да е non-blocking спрямо обработката, но да има кратък lock)

---

### 16) (Свободен теоретичен) RCU Pub/Sub: защо `publish()` е „wait-free“ (в смисъла на кода)?
Фрагмент:
```cpp
auto node = std::atomic_load_explicit(&head, std::memory_order_acquire);
while (node) {
    node->callback(event);
    node = node->next;
}
```

**Въпрос:** Обяснете защо `publish()` не блокира върху mutex и как RCU идеята позволява конкурентно `subscribe()` без да „чупи“ traversal-а. Посочете 2 ключови думи.

**Верен отговор / ключови думи:**
- Няма mutex; `publish` прави **read-only traversal**.
- `subscribe` прави **copy/update на head** чрез CAS (RCU: Read-Copy-Update).
- Ключови думи: **immutable nodes**, **atomic head**, **CAS**, **acquire/release**, **shared_ptr lifetime**.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „RCU е lock-free.“ (твърде общо; търси се защо traversal-ът е стабилен при concurrent insert)

---

### 17) (Свободен теоретичен) Hybrid SPSC queue: нарушава ли се SPSC договорът?
Фрагмент:
```cpp
class Worker {
    SPSCQueue<std::function<void()>> tasks;
    bool submit(std::function<void()>&& task) {
        return tasks.enqueue(task);
    }
};

class LockFreeThreadPool {
    bool submit(F&& task) {
        // ... избира worker и вика workers[index]->submit(...)
    }
};
```

**Въпрос:** `SPSCQueue` означава Single Producer / Single Consumer. Тук consumer е worker нишката. Кой е producer-ът? Може ли да има **повече от 1 producer** за един worker и ако да — при каква употреба? Какво трябва да се промени, за да е коректно в общия случай?

**Верен отговор / ключови думи:**
- Producer е нишката (или нишките), които викат `LockFreeThreadPool::submit`.
- Ако `submit` може да се вика от множество нишки едновременно, тогава за даден worker queue има **MPSC** (multiple producers) → нарушава SPSC договора.
- В този демо-код main е единствен producer (възможно е да е OK), но като библиотека е риск.
- Поправка: MPSC queue (или mutex около per-worker enqueue), или thread-affine submit, или central MPMC queue.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Не, защото има атомици.“ (SPSC/MPSC е структурен договор, не просто „има атомици“)

---

### 18) (Свободен теоретичен) False sharing: какво точно предпазва `alignas(64)` в `SPSCQueue`?
Фрагмент:
```cpp
struct alignas(64) {
    std::atomic<size_t> value{0};
} head, tail;
```

**Въпрос:** Обяснете какво е false sharing в този конкретен случай и защо би се проявило именно между `head` и `tail` без alignment. Дайте 2-3 изречения.

**Верен отговор / ключови думи:**
- Producer често пише `tail`, consumer често пише `head`.
- Ако `head` и `tail` са в една и съща cache line, всяка промяна invalidates line-а за другия core → **cache ping-pong**.
- `alignas(64)` ги слага на отделни cache lines, намалява invalidations и подобрява throughput.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Alignment прави достъпа по-бърз.“ (липсва конкретният механизъм cache invalidation между producer/consumer)

---

### 19) (Свободен теоретичен) Pub/Sub: защо се прави копие на callback-ите под mutex?
Фрагмент:
```cpp
using Callback = std::function<void(int)>;

std::vector<Callback> callbacks;
std::mutex m;

void publish(int event) {
    std::vector<Callback> local;
    {
        std::lock_guard<std::mutex> lock(m);
        local = callbacks; // snapshot
    }

    // Идеята: изпълняваме callback-ите извън mutex-а.
    for (const auto& cb : local) {
        cb(event);
    }
}
```

**Въпрос:** Обяснете защо не е добра идея mutex-ът да се държи заключен по време на изпълнение на callback-ите. Дайте 2 конкретни последствия.

**Верен отговор / ключови думи:**
- Ако държиш mutex-а докато изпълняваш callback-ите:
  - блокираш `subscribe()` (и евентуално други `publish`-и) за целия период на обработка,
  - риск от deadlock/re-entrancy: callback може да се опита да subscribe/publish и да чака същия mutex.
- Copy-on-publish позволява кратък lock и паралелна обработка извън критична секция.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „За да е thread-safe.“ (вярно, но недостатъчно; търсят се конкретните последствия: contention + re-entrancy/deadlock)

---
 
## Банка въпроси (20–80)
 
### 20) (Избираем отговор) Data race или не?
Фрагмент:
```cpp
int x = 0;
std::atomic<bool> ready{false};

// Thread A
x = 42;
ready.store(true, std::memory_order_release);

// Thread B
while (!ready.load(std::memory_order_acquire)) {}
std::cout << x << "\n";
```

**Въпрос:** Кое е вярно?

A) Има data race върху `x`.

B) Няма data race; `x` е защитен от release/acquire върху `ready`.

C) Няма data race само ако `ready` е `seq_cst`.

D) Зависи от компилатора.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Трябва mutex, иначе има race.“

---
 
### 21) (Избираем отговор) `compare_exchange_weak` и стойността на `expected`
Фрагмент:
```cpp
std::atomic<int> v{5};
int expected = 4;
bool ok = v.compare_exchange_weak(expected, 9);
```

**Въпрос:** Ако операцията върне `false`, кое е най-вероятно вярно за `expected`?

A) Остава 4.

B) Става 5 (текущата стойност на `v`).

C) Става 9.

D) Става неопределена стойност.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „expected е само входен параметър и не се променя.“

---
 
### 22) (Избираем отговор) Condition variable: правилен шаблон
Фрагмент:
```cpp
std::mutex m;
std::condition_variable cv;
bool ready = false;
```

**Въпрос:** Кой фрагмент е най-коректен (спрямо spurious wakeups)?

A)
```cpp
std::unique_lock<std::mutex> lk(m);
cv.wait(lk);
```

B)
```cpp
std::unique_lock<std::mutex> lk(m);
cv.wait(lk, [&]{ return ready; });
```

C)
```cpp
if (!ready) cv.wait(std::unique_lock<std::mutex>(m));
```

D)
```cpp
while (!ready) cv.wait(std::unique_lock<std::mutex>(m));
```

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Всички са еднакво правилни.“

---
 
### 23) (Избираем отговор) Деструктор на thread pool: какво е задължително?
Фрагмент:
```cpp
~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m);
        stop = true;
    }
    // ???
    for (auto& w : workers) w.join();
}
```

**Въпрос:** Кое липсва най-логично на мястото на `// ???`?

A) `condition.notify_one();`

B) `condition.notify_all();`

C) `std::this_thread::yield();`

D) Нищо; `join()` е достатъчно.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „notify_one е достатъчно във всички случаи.“

---
 
### 24) (Избираем отговор) `std::atomic` и memory ordering
**Въпрос:** Кое твърдение е най-точно?

A) `memory_order_relaxed` забранява всяко reordering.

B) `memory_order_relaxed` гарантира атомарност, но не гарантира ред/видимост на други записи.

C) `memory_order_release` се използва само за load.

D) `memory_order_acquire` се използва само за store.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „relaxed е небезопасен и винаги води до грешка.“

---
 
### 25) (Избираем отговор) False sharing
**Въпрос:** Кое описание най-точно улавя false sharing?

A) Две нишки четат една и съща променлива.

B) Две нишки пишат различни променливи, които попадат в една и съща cache line.

C) Една нишка пише, друга нишка чете същата променлива.

D) Две нишки пишат в различни кешове.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „False sharing е същото като data race.“

---
 
### 26) (Избираем отговор) Coroutine: къде живеят локалните променливи?
**Въпрос:** При C++ coroutine, локалните променливи, които са живи през `co_await`, обикновено се съхраняват:

A) На stack-а на caller-а.

B) В coroutine frame (heap-алокация или еквивалент).

C) В регистрите.

D) В TLS.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „На stack-а, защото това е функция.“

---
 
### 27) (Избираем отговор) „Non-blocking“ в контекст на API
**Въпрос:** Кое описание е най-коректно за „non-blocking publish“?

A) `publish()` никога не взима mutex.

B) `publish()` не чака обработката на subscriber-ите да завърши.

C) `publish()` не алокира памет.

D) `publish()` винаги използва atomics.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Non-blocking означава lock-free.“

---
 
### 28) (Избираем отговор) Потенциален deadlock
Фрагмент:
```cpp
std::mutex a, b;

// Thread 1
std::lock_guard<std::mutex> l1(a);
std::lock_guard<std::mutex> l2(b);

// Thread 2
std::lock_guard<std::mutex> r1(b);
std::lock_guard<std::mutex> r2(a);
```

**Въпрос:** Кое е вярно?

A) Няма риск от deadlock, защото `lock_guard` е RAII.

B) Има риск от deadlock при определено interleaving.

C) Има риск само ако mutex-ите са recursive.

D) Риск има само при `std::unique_lock`.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „RAII елиминира deadlock.“

---
 
### 29) (Избираем отговор) Backpressure: най-пряко решение
**Въпрос:** В система producer→queue→consumer, producer е по-бърз от consumer. Кое е най-пряко решение за backpressure?

A) Увеличавате `std::cout` логовете.

B) Ограничавате капацитета на опашката (bounded queue) и блокирате/отказвате при пълна опашка.

C) Премахвате mutex-ите.

D) Изключвате exception-ите.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Просто увеличете RAM.“

---
 
### 30) (Избираем отговор) Ред на съобщенията при паралелна обработка
**Въпрос:** При Pub/Sub, ако изпращате събития към subscriber-ите чрез thread pool, кое твърдение е най-точно?

A) Редът на обработка е винаги същият като реда на publish.

B) Редът на обработка може да се наруши без допълнителен механизъм.

C) Редът се гарантира автоматично от `std::thread`.

D) Редът се гарантира, ако използвате `memory_order_seq_cst`.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Thread pool автоматично пази реда.“

---
 
### 31) (Избираем отговор) ABA проблем
**Въпрос:** Кое е най-точно описание на ABA проблема?

A) Кеш-линиите се пренареждат.

B) Стойност се променя A→B→A и CAS операцията може да „повярва“, че нищо не се е променило.

C) Две нишки пишат едновременно.

D) Това е проблем само в Java.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „ABA е просто data race.“

---
 
### 32) (Избираем отговор) Кога `detach()` е опасно?
**Въпрос:** Кое е най-честият риск при `std::thread::detach()`?

A) По-бавно изпълнение.

B) Изтичане на памет в ОС.

C) Достъп до обекти с изтекъл lifetime (dangling references/pointers).

D) Принтиране в грешен ред.

**Верен отговор:** C)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „detach е безопасно, защото ОС управлява нишката.“

---
 
### 33) (Избираем отговор) Разлика паралелизъм vs конкурентност
**Въпрос:** Кое твърдение е най-точно?

A) Конкурентност означава задължително множество ядра.

B) Паралелизъм е едновременно изпълнение на повече от една задача в един и същи момент.

C) Конкурентност и паралелизъм са синоними.

D) Паралелизъм е само за I/O.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „И двете означават едно и също: много нишки.“

---
 
### 34) (Избираем отговор) `std::atomic_flag`
**Въпрос:** За какво най-често се използва `std::atomic_flag`?

A) За бариера.

B) За спинлок.

C) За условна променлива.

D) За thread-local storage.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „atomic_flag е просто bool.“

---
 
### 35) (Избираем отговор) „Остатък“ при разпределяне на работа
Фрагмент:
```cpp
int total = producers * per_producer;
int per_consumer = total / consumers;
// стартирате точно consumers нишки, всяка чете per_consumer елемента
```

**Въпрос:** Кое е задължително да добавите, за да няма загубени елементи при неравномерно деление?

A) Нищо.

B) Да разпределите остатъка (напр. последният consumer да поеме `total - per_consumer*(consumers-1)`).

C) Да смените `int` с `double`.

D) Да добавите `memory_order_seq_cst`.

 **Верен отговор:** B)
 
 **Какво вероятно ще отговори стандартен LLM (червен флаг):**
 - „Няма проблем; целочисленото деление е достатъчно.“

---

### 36) (Избираем отговор) Защо `notify_all()` понякога е нужно?
**Въпрос:** Кога `notify_all()` е по-подходящо от `notify_one()`?

A) Винаги.

B) При shutdown, когато всички waiting worker-и трябва да се събудят.

C) Никога.

D) Само при `std::future`.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „notify_one е винаги по-ефективно и достатъчно.“

---

### 37) (Избираем отговор) Видимост на writes при `release`
**Въпрос:** `store(memory_order_release)` гарантира, че:

A) Следващите операции в същата нишка няма да се пренаредят преди store-а.

B) Предишните операции в същата нишка няма да се пренаредят след store-а.

C) Нищо не гарантира.

D) Гарантира само върху x86.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „release означава ‘освобождава mutex’.“

---

### 38) (Избираем отговор) Busy-wait и spinlock
**Въпрос:** Кое е типичен недостатък на spinlock спрямо mutex?

A) Използва повече памет.

B) Може да гори CPU в busy-wait при contention.

C) Не може да работи с атомици.

D) Винаги е по-бавен.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Spinlock никога не товари CPU.“

---

### 39) (Избираем отговор) „Wait-free“ дефиниция
**Въпрос:** Кое твърдение е най-точно?

A) Wait-free: няма I/O.

B) Wait-free: всяка операция завършва за ограничен брой стъпки, независимо от други нишки.

C) Wait-free: няма mutex.

D) Wait-free: няма memory allocation.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Wait-free е същото като lock-free.“

---

### 40) (Избираем отговор) Coroutine: `initial_suspend`
**Въпрос:** Ако `initial_suspend()` връща `std::suspend_always`, кое е вярно?

A) Coroutine започва да изпълнява веднага.

B) Coroutine се създава, но не започва, докато не бъде `resume()`-ната.

C) Coroutine се изпълнява на отделна нишка.

D) Coroutine не може да `co_return`.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „initial_suspend е само оптимизация и не променя поведението.“

---

### 41) (Избираем отговор) Подравняване и false sharing
**Въпрос:** `alignas(64)` най-често се използва, за да:

A) се намали размерът на обекта.

B) се избегне false sharing при чести writes от различни нишки.

C) се избегне ABA.

D) се подобри exception safety.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „alignas винаги прави кода по-бърз.“

---

### 42) (Избираем отговор) Идемпотентност при доставка „поне веднъж“
**Въпрос:** Защо идемпотентните операции са важни при at-least-once доставка?

A) За да са по-бързи.

B) За да могат да се изпълнят повторно без допълнителни странични ефекти при дубликати.

C) За да не се използва mutex.

D) За да се гарантира ordering.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Идемпотентност е свързана само с HTTP.“

---

### 43) (Избираем отговор) Кога `std::thread::hardware_concurrency()` може да върне 0?
**Въпрос:** Кое е вярно?

A) Никога.

B) Може да върне 0, ако информацията не е налична.

C) Връща броя активни нишки.

D) Връща броя на NUMA възли.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Винаги връща реалния брой ядра.“

---

### 44) (Избираем отговор) Критична секция в worker цикъл
**Въпрос:** Кое е добра практика в thread pool worker цикъл?

A) Да изпълнявате задачата вътре в mutex-а.

B) Да извличате задачата под lock, но да я изпълнявате извън lock.

C) Да държите lock по време на `sleep_for`.

D) Да `notify_all()` при всяка добавена задача.

**Верен отговор:** B)

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „По-безопасно е задачата да се изпълнява под lock.“

---

### 45) (Свободен теоретичен) Проектиране на bounded queue
**Въпрос:** Проектирайте bounded queue за thread pool, която:

- блокира producer-а при пълна опашка,
- позволява shutdown без deadlock,
- не губи задачи.

Опишете с 5-8 точки нужните променливи (mutex/cv/флагове/броячи) и основната логика.

**Верен отговор / ключови думи:**
- `capacity`, `queue`, `mutex`, два `condition_variable` (not_empty, not_full) или един с predicate.
- `closed/stop` флаг.
- `push/enqueue`: чака `size < capacity || closed`, отказ/exception при `closed`.
- `pop/dequeue`: чака `!queue.empty() || closed`, връща „няма повече“ при `closed && empty`.
- `close()`: set флаг + `notify_all()`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Слагате mutex и става.“

---

### 46) (Свободен теоретичен) Ordering при паралелно обработване
**Въпрос:** Имате последователност от събития с `id=1..N`. Subscriber трябва да ги обработи строго по `id`. В същото време искате паралелизъм.

Предложете подход (1-2 варианта) и обяснете компромиса.

**Верен отговор / ключови думи:**
- Partitioning по ключ (напр. `id % K`) → ordering в партиция.
- Per-subscriber single-threaded executor / serial queue.
- Sequence numbers + буфериране и „reorder buffer“.
- Компромис: latency/памет/throughput.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Thread pool гарантира реда, ако използвате mutex.“

---

### 47) (Свободен теоретичен) Кога lock-free не е по-добре?
**Въпрос:** Посочете 3 причини, поради които lock-free структура може да е по-лош избор от lock-based в реална система.

**Верен отговор / ключови думи:**
- Сложност/висок риск от bug.
- Memory reclamation проблеми (hazard pointers/epochs).
- По-висок CPU разход при contention (retry loops).
- По-труден debugging/тестове.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Lock-free винаги е по-бързо.“

---

### 48) (Свободен теоретичен) Release/Acquire: минимално обяснение
**Въпрос:** Обяснете с 3 изречения какво означава „release-acquire pair“ и какъв тип грешка предотвратява.

**Верен отговор / ключови думи:**
- `store(release)` публикува предишните writes.
- `load(acquire)` гарантира видимост на тези writes след като види флага.
- Предотвратява bug тип „виждам флага, но не виждам данните“.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Това са бариери и толкова.“

---

### 49) (Свободен теоретичен) „Exactly-once“: защо е трудно?
**Въпрос:** Опишете 3 причини защо exactly-once доставка е трудна в асинхронни системи (дори локално при crash).

**Верен отговор / ключови думи:**
- Дубликати при retry.
- Загуба на ack/съобщение при crash.
- Транзакционност между „получих“ и „обработих“.
- Нужда от deduplication store / idempotency.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Слагате mutex и няма дубликати.“

---

### 50) (Свободен теоретичен) „Ownership“ договор в thread pool API
**Въпрос:** Имате обект `ThreadPool pool;` и други нишки, които могат да викат `pool.enqueue(...)`. Какъв минимален „договор“ бихте описали, за да няма UB при destruction? Дайте 2 варианта.

**Верен отговор / ключови думи:**
- Вариант 1: `pool` се унищожава само след като всички producer-и са спрели (lifetime rule).
- Вариант 2: `shutdown()/close()` + `enqueue` отказва (връща `false`/хвърля) след shutdown; външните нишки спират да викат.
- Вариант 3: споделен ownership (`shared_ptr<ThreadPool>`), но пак с `close` семантика.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „C++ автоматично се грижи за lifetime.“

---

### 51) (Свободен теоретичен) Отказ от `detach()`
**Въпрос:** Дайте 2 техники да избегнете `detach()` и да запазите асинхронно поведение.

**Верен отговор / ключови думи:**
- Thread pool.
- `std::jthread` + `stop_token`.
- Event loop + coroutines.
- `std::async`/futures (с уговорки за policy/lifetime).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „detach е окей, стига да работи.“

---

### 52) (Свободен теоретичен) Защо `std::cout` „разкъсва“ изхода?
**Въпрос:** Обяснете защо паралелен принт от много нишки често води до „разкъсани“ редове и как бихте го направили по-четим (2 идеи).

**Верен отговор / ключови думи:**
- Интерливинг на writes към stdout.
- Буфериране/flush.
- Локален `std::ostringstream` + еднократен print под mutex.
- Един логер thread.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Проблемът е, че CPU е бавен.“

---

### 53) (Свободен теоретичен) Какво е contention и как го намалявате?
**Въпрос:** Дайте 3 конкретни техники за намаляване на contention в система с много worker-и.

**Верен отговор / ключови думи:**
- Per-worker queues.
- Sharding/partitioning по ключ.
- Намаляване на критичната секция.
- Batch-ване.
- Work stealing.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Използвайте повече нишки.“

---

### 54) (Свободен теоретичен) Coroutine dangling reference
**Въпрос:** Опишете с кратък пример защо `co_await` + capture-by-reference към локална променлива на caller може да е опасно. Каква е типичната поправка?

**Верен отговор / ключови думи:**
- След suspend caller може да приключи, референцията става dangling.
- Поправка: capture by value / `shared_ptr` / прехвърляне на данните в coroutine frame.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Референциите са бързи и затова са по-добри.“

---

### 55) (Свободен теоретичен) Разлика blocking / non-blocking
**Въпрос:** Дайте дефиниция и по 1 пример за blocking и non-blocking синхронизация.

 **Верен отговор / ключови думи:**
 - Blocking: mutex/condvar.
 - Non-blocking: CAS loop/lock-free queue/spinlock (не блокира, но може да върти).
 
 **Какво вероятно ще отговори стандартен LLM (червен флаг):**
 - „Non-blocking означава без никакво чакане.“

---

### 56) (Свободен теоретичен) Кога `seq_cst` е оправдан?
**Въпрос:** Посочете 2 ситуации, в които бихте предпочели `memory_order_seq_cst` вместо acquire/release. Обосновете с по 1 изречение.

**Верен отговор / ключови думи:**
- Простота/коректност при сложни interleavings и много атомици.
- Когато ви трябва един глобален ред на събитията (напр. за логика/инварианти/дебъг).
- Когато производителността не е критична и искате най-ясния модел.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „seq_cst винаги е най-добрият избор.“

---

### 57) (Свободен теоретичен) Оптимизация: batch-ване на дребни задачи
**Въпрос:** Защо batch-ване на задачи (напр. групиране на 10 задачи в една) често ускорява thread pool при много дребни задачи? Дайте 2 причини.

**Верен отговор / ключови думи:**
- Намалява overhead от lock/notify/context-switch.
- Подобрява locality и намалява contention върху опашката.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Защото повече код означава по-бързо.“

---

### 58) (Свободен теоретичен) Защо „повече нишки“ не гарантира ускорение?
**Въпрос:** Имате 1000 задачи, но всяка задача държи един и същ mutex за 90% от времето си. Какво ще наблюдавате при увеличаване на броя worker-и и защо?

**Верен отговор / ключови думи:**
- Слабо скалиране/насищане: throughput почти не расте след малък брой нишки.
- Причина: сериализация около mutex (контеншън) + контекстни превключвания.
- Amdahl’s law: голяма последователна част доминира.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Повече нишки винаги означават линейно ускорение.“

---

### 59) (Свободен теоретичен) Опасност при „callback под lock“
**Въпрос:** Защо е опасно да извиквате потребителски callback докато държите вътрешен mutex? Дайте 2 конкретни риска.

**Верен отговор / ключови думи:**
- Re-entrancy: callback може да извика обратно в същия обект (`subscribe/publish`) → deadlock.
- Непредсказуемо/дълго време в критичната секция → contention и latency.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Mutex прави всичко безопасно.“

---

### 60) (Свободен теоретичен) Memory reclamation в lock-free структури
**Въпрос:** При lock-free linked list/queue, защо не можете просто да `delete`-нете възела веднага след като го „махнете“ с CAS? Обяснете с 2-3 изречения.

**Верен отговор / ключови думи:**
- Друга нишка може да държи pointer към същия възел (прочетен преди CAS).
- CAS успех не означава, че никой не го чете.
- Трябват техники като hazard pointers/epoch reclamation/ref-count.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Щом CAS е успял, никой друг не го използва.“

---

### 61) (Свободен теоретичен) SPSC срещу MPSC: две последствия
**Въпрос:** Обяснете разликата между SPSC и MPSC опашка и как това променя алгоритъма (2 ключови последствия).

**Верен отговор / ключови думи:**
- При MPSC имате повече от 1 producer → нужда от CAS/допълнителна синхронизация (особено за `tail`).
- SPSC може да избегне CAS и да разчита на по-лесни invariants (една нишка пише, една чете).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Няма разлика; и двете са lock-free.“

---

### 62) (Свободен теоретичен) Thread pool fairness и „дълги задачи“
**Въпрос:** Какво означава „fairness“ при разпределяне на задачи и защо round-robin може да е лошо при различни по тежест задачи?

**Верен отговор / ключови думи:**
- Fairness: да не се „гладува“ задача/worker и да има баланс.
- Ако една тежка задача попадне на worker, round-robin може да остави този worker блокиран дълго.
- Work stealing/динамично разпределение подобрява баланса.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Round-robin винаги е оптимално.“

---

### 63) (Свободен теоретичен) Намиране на bottleneck в thread pool
**Въпрос:** Имате thread pool, но throughput не расте над 4 worker-и. Предложете 3 конкретни неща, които бихте измерили/проверили.

**Верен отговор / ключови думи:**
- Contention върху queue mutex / време в критични секции.
- Фалшиво споделяне (cache-line ping-pong).
- Overhead от твърде дребни задачи.
- I/O/blocking вътре в задачи.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Добавете още RAM/CPU.“

---

### 64) (Свободен теоретичен) Coroutines vs threads: кога кое?
**Въпрос:** Дайте правило (и 1 изключение) кога бихте предпочели coroutines и кога threads.

**Верен отговор / ключови думи:**
- Coroutines: много конкурентни I/O-bound операции, където се чака.
- Threads: CPU-bound паралелизъм.
- Изключение: coroutines върху thread pool (CPU-bound части пак се изпълняват в worker нишки).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Винаги coroutines, защото са по-евтини.“

---

### 65) (Писане на код) Thread-safe bounded queue
**Въпрос:** Напишете (псевдокод или C++) клас `BoundedQueue<T>`, който има:

- `bool push(T)` – блокира, докато има място; връща `false`, ако е затворена.
- `std::optional<T> pop()` – блокира, докато има елемент; връща празно при затворена и празна.
- `void close()`.

**Верен отговор / ключови думи:**
- `mutex`, `condition_variable`, `queue`, `capacity`, `closed`.
- `wait` с predicate и за двете операции.
- `notify_all` в `close()`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „push/pop с mutex без close семантика.“

---

### 66) (Писане на код) Минимален thread pool интерфейс
**Въпрос:** Напишете минимален интерфейс на thread pool с методи:

- `enqueue(std::function<void()>)`
- `shutdown()`

Гаранция: `enqueue` след `shutdown` не приема задачи.

Не е нужно да пишете целия код, но опишете ключовите член-данни и синхронизация.

**Верен отговор / ключови думи:**
- `workers`, `queue`, `mutex`, `cv`, `stop/closed`.
- worker loop: wait predicate, pop, unlock, execute.
- shutdown: set flag + `notify_all` + join.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Пуснете 100 нишки и това е pool.“

---

### 67) (Писане на код) Lock-free counter с CAS
**Въпрос:** Напишете функция `void increment(std::atomic<int>& x)`, която увеличава `x` с 1 чрез CAS loop. Уточнете какво правите с променливата `expected`.

**Верен отговор / ключови думи:**
- `expected = x.load(relaxed)`.
- loop: `compare_exchange_weak(expected, expected+1, ...)`.
- При failure `expected` се обновява от CAS и retry-вате.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „`x++` върху atomic е същото и не ви трябва loop.“

---

### 68) (Писане на код) Бариера за N нишки
**Въпрос:** Напишете бариера `Barrier(N)`, която има метод `arrive_and_wait()`. Всички нишки трябва да чакат, докато N на брой не извикат метода, след което всички продължават.

Опишете нужните променливи и логика.

**Верен отговор / ключови думи:**
- `mutex`, `condition_variable`, брояч `count`.
- „generation“/`phase`, за да може бариерата да е reusable.
- Последният пристигнал: reset на брояча + increment на phase + `notify_all`.
- Останалите: `wait` на `phase`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Ползвайте `sleep_for` докато всички дойдат.“

---

### 69) (Свободен теоретичен) Work stealing: защо помага?
**Въпрос:** Обяснете с 3-5 изречения какво е work stealing и защо често подобрява баланса при различни по тежест задачи.

**Верен отговор / ключови думи:**
- Всеки worker има локална опашка.
- Когато worker остане без работа, „краде“ от други.
- Намалява contention спрямо една централизирана опашка и подобрява load balancing.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Work stealing е просто да добавите още нишки.“

---

### 70) (Свободен теоретичен) Договор за доставка в Pub/Sub
**Въпрос:** Изберете една конкретна гаранция за доставка (at-most-once, at-least-once или exactly-once) за локален async Pub/Sub (в един процес). Опишете:

- какви 2 механизма ще добавите (напр. ack/ретрай/dedup/idempotency),
- и каква цена плащате (латентност/памет/сложност).

**Верен отговор / ключови думи:**
- At-most-once: без retry → риск от загуба.
- At-least-once: retry + idempotency/dedup ключ.
- Exactly-once: трудно; нужни транзакционни/идемпотентни операции + устойчив state.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Exactly-once става, ако сложите един mutex около publish.“

---

### 71) (Свободен теоретичен) `notify_one` и underutilization
**Въпрос:** Имате 4 worker-а и producer, който enqueue-ва 100 задачи накуп. В кои два случая може да наблюдавате „недоизползване“ (само 1 worker работи дълго), въпреки че викате `notify_one()` за всяка задача?

**Верен отговор / ключови думи:**
- Ако worker-ите не чакат на `cv` (напр. busy-loop) и няма коректен predicate, може да има пропуснати събуждания/грешна логика.
- Ако извличането на задачи и изпълнението стават под lock (лош дизайн), един worker може да „монополизира“ queue mutex.
- Ако задачите са неравномерни и няма work stealing, един worker може да поеме тежките.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „notify_one винаги буди всички нужни нишки.“

---

### 72) (Поправяне на грешки) Изгубено събуждане / spurious wakeup
Фрагмент:
```cpp
std::mutex m;
std::condition_variable cv;
bool ready = false;

void consumer() {
    std::unique_lock<std::mutex> lk(m);
    if (!ready) {
        cv.wait(lk);
    }
    use();
}
```

**Въпрос:** Посочете каква е грешката и дайте минимална корекция.

**Верен отговор / ключови думи:**
- `if` трябва да е `while`, или `cv.wait(lk, [&]{ return ready; })`.
- Причина: spurious wakeups и/или пропуснато условие.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Добавете още `notify_all()` и проблемът изчезва.“

---

### 73) (Поправяне на грешки) Dangling reference в асинхронна задача
Фрагмент:
```cpp
std::function<void()> make_task() {
    std::string s = "hello";
    return [&] { std::cout << s << "\n"; };
}
```

**Въпрос:** Защо е грешно да изпълните върнатата задача по-късно в друга нишка? Поправете кода.

**Верен отговор / ключови думи:**
- `s` излиза от scope → lambda държи dangling reference.
- Поправка: capture by value `[s]` (или move) и без референция.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Референцията е валидна, защото lambda я пази.“

---

### 74) (Поправяне на грешки) Грешен `get()` за coroutine с повече от 1 suspension
Фрагмент:
```cpp
T get() {
    if (!h.done()) h.resume();
    return h.promise().value;
}
```

**Въпрос:** Защо това е логически грешно, ако coroutine може да suspend-ва повече от веднъж? Дайте минимална корекция на идеята.

**Верен отговор / ключови думи:**
- Едно `resume()` не гарантира `done()`; `value` може да не е установена.
- Трябва да resume-вате в цикъл/през scheduler до `h.done()`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Добавете mutex.“

---

### 75) (Поправяне на грешки) Deadlock чрез callback под lock
Фрагмент:
```cpp
std::mutex m;
std::vector<std::function<void()>> subs;

void publish() {
    std::lock_guard<std::mutex> lk(m);
    for (auto& cb : subs) {
        cb();
    }
}
```

**Въпрос:** Посочете 2 проблема и покажете минимална поправка.

**Верен отговор / ключови думи:**
- Callback може да извика `subscribe/publish` → deadlock.
- Дълга критична секция (непредсказуемо време).
- Поправка: копие на `subs` под lock, изпълнение извън lock.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Всичко е наред, защото има mutex.“

---

### 76) (Поправяне на грешки) SPSC опашка използвана като MPSC
Фрагмент:
```cpp
// submit може да бъде викано от много нишки
bool submit_to_worker(SPSCQueue<Task>& q, Task t) {
    return q.enqueue(t);
}
```

**Въпрос:** Какъв е проблемът и кои са 2 възможни поправки?

**Верен отговор / ключови думи:**
- Нарушен договор: множество producers към SPSC.
- Поправка 1: MPSC queue.
- Поправка 2: mutex около `enqueue` или гарантиране на single-producer (thread-affinity).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Щом е atomic, няма значение.“

---

### 77) (Поправяне на грешки) Липсва `notify_all` при shutdown
Фрагмент:
```cpp
void stop_pool() {
    {
        std::lock_guard<std::mutex> lk(m);
        stop = true;
    }
    // няма notify
}
```

**Въпрос:** Какво може да се случи и каква е корекцията?

**Верен отговор / ключови думи:**
- Worker-и могат да останат блокирани на `cv.wait` → deadlock при `join()`.
- Корекция: `cv.notify_all()`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „join() ще ги събуди.“

---

### 78) (Поправяне на грешки) Неправилен memory order при публикация
Фрагмент:
```cpp
std::atomic<bool> ready{false};
int data;

// producer
data = 7;
ready.store(true, std::memory_order_relaxed);

// consumer
while (!ready.load(std::memory_order_relaxed)) {}
std::cout << data << "\n";
```

**Въпрос:** Какъв е проблемът и каква е минималната поправка (memory order-ите)?

**Верен отговор / ключови думи:**
- Липсва happens-before; consumer може да види `ready=true`, но да не види `data=7`.
- Поправка: producer `store(release)`, consumer `load(acquire)`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Това винаги работи на x86, значи е правилно.“

---

### 79) (Поправяне на грешки) Неинициализирана стойност в promise
Фрагмент:
```cpp
struct promise_type {
    int value; // неинициализиран
    void return_value(int v) { value = v; }
};
```

**Въпрос:** Защо е опасно да прочетете `value` преди coroutine да е `done()`? Дайте 1 поправка.

**Верен отговор / ключови думи:**
- Може да се прочете неинициализирана стойност.
- Поправка: четете само след `done()`; или `std::optional<int> value;`.

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „Компилаторът ще го инициализира на 0.“

---

### 80) (Поправяне на грешки) Data race с `std::vector`
Фрагмент:
```cpp
std::vector<int> v;

// Thread A
v.push_back(1);

// Thread B
v.push_back(2);
```

**Въпрос:** Какъв е проблемът и кои са 2 възможни решения?

**Верен отговор / ключови думи:**
- Data race + възможна reallocation.
- Решение 1: mutex.
- Решение 2: предварително `reserve` + атомичен индекс + запис по индекс (ако е приложимо).

**Какво вероятно ще отговори стандартен LLM (червен флаг):**
- „vector е thread-safe за `push_back`.“

---
