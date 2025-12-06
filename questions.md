# Въпроси по "Модерни техники за паралелизъм и асинхронност в C++"

## Съдържание

- [Въпроси с избираем отговор (1-40)](#въпроси-с-избираем-отговор)
- [Отворени въпроси (41-70)](#отворени-въпроси)
- [Въпроси с код (71-100)](#въпроси-с-код)

---

## Въпроси с избираем отговор

### Въпрос 1

Каква е основната разлика между паралелизъм и асинхронност?

A) Паралелизмът е по-бърз от асинхронността  
B) Паралелизмът изпълнява задачи едновременно на множество ядра, докато асинхронността чередува задачи  
C) Асинхронността изисква повече памет  
D) Няма разлика между двете понятия  

**Отговор:** B) Паралелизмът изпълнява задачи едновременно на множество ядра, докато асинхронността чередува задачи

---

### Въпрос 2

Колко пъти по-бързо е използването на Thread Pool в сравнение със създаването на нови нишки за 1000 задачи?

A) 2-5 пъти  
B) 10-20 пъти  
C) 50-100 пъти  
D) 1000 пъти  

**Отговор:** C) 50-100 пъти (Thread Pool ≈ 1-2ms, създаване на нишки ≈ 50-100ms)

---

### Въпрос 3

Кой от следните НЕ е проблем при наивния подход за създаване на нова нишка за всяка задача?

A) System call за създаване  
B) Заделяне на stack памет  
C) Автоматично балансиране на натоварването  
D) Context switching overhead  

**Отговор:** C) Автоматично балансиране на натоварването

---

### Въпрос 4

Какъв е типичният размер на stack паметта за една нишка (thread)?

A) 1-10 KB  
B) 100-500 KB  
C) 1-2 MB  
D) 10-20 MB  

**Отговор:** C) 1-2 MB

---

### Въпрос 5

Какъв е типичният размер на coroutine frame?

A) По-малко от 1 KB  
B) 100 KB - 500 KB  
C) 1-2 MB  
D) Зависи от операционната система  

**Отговор:** A) По-малко от 1 KB

---

### Въпрос 6

Коя ключова дума в C++20 се използва за suspend и чакане на резултат в coroutine?

A) `co_return`  
B) `co_yield`  
C) `co_await`  
D) `suspend`  

**Отговор:** C) `co_await`

---

### Въпрос 7

Какво е предимството на lock-free структурите пред lock-based?

A) По-лесна имплементация  
B) Non-blocking и по-висока throughput  
C) По-малко използване на памет  
D) По-добра съвместимост с legacy код  

**Отговор:** B) Non-blocking и по-висока throughput

---

### Въпрос 8

Какво означава CAS в контекста на lock-free програмиране?

A) Cache Allocation System  
B) Compare-And-Swap  
C) Concurrent Access Synchronization  
D) Critical Area Section  

**Отговор:** B) Compare-And-Swap

---

### Въпрос 9

Кой memory order гарантира най-силна синхронизация, но е най-бавен?

A) `memory_order_relaxed`  
B) `memory_order_acquire`  
C) `memory_order_release`  
D) `memory_order_seq_cst`  

**Отговор:** D) `memory_order_seq_cst`

---

### Въпрос 10

Какво е предназначението на `std::condition_variable` в Thread Pool?

A) Да заключва опашката  
B) Да събужда worker нишки когато има нови задачи  
C) Да брои задачите  
D) Да управлява паметта  

**Отговор:** B) Да събужда worker нишки когато има нови задачи

---

### Въпрос 11

В Publisher/Subscriber pattern, какво е ролята на Event Broker?

A) Генерира събития  
B) Обработва събития  
C) Посредник между publishers и subscribers  
D) Съхранява събития в база данни  

**Отговор:** C) Посредник между publishers и subscribers

---

### Въпрос 12

Коя гаранция за доставка е най-бърза, но може да загуби съобщения?

A) At-least-once  
B) Exactly-once  
C) At-most-once  
D) Guaranteed delivery  

**Отговор:** C) At-most-once

---

### Въпрос 13

Какво е backpressure в контекста на Pub/Sub системи?

A) Натиск върху паметта  
B) Publisher генерира събития по-бързо отколкото subscribers могат да обработят  
C) Твърде много subscribers  
D) Мрежови проблеми  

**Отговор:** B) Publisher генерира събития по-бързо отколкото subscribers могат да обработят

---

### Въпрос 14

Какво означава RCU в контекста на lock-free структури?

A) Read-Copy-Update  
B) Remote Control Unit  
C) Recursive Call Utility  
D) Reference Counting Unit  

**Отговор:** A) Read-Copy-Update

---

### Въпрос 15

Кой тип scheduling използват coroutines?

A) Преемптивен (preemptive)  
B) Кооперативен (cooperative)  
C) Round-robin  
D) Priority-based  

**Отговор:** B) Кооперативен (cooperative)

---

### Въпрос 16

Колко е типичното време за context switch при threads?

A) 1-10 наносекунди  
B) 10-100 наносекунди  
C) 1-10 микросекунди  
D) 1-10 милисекунди  

**Отговор:** C) 1-10 микросекунди

---

### Въпрос 17

Колко е типичното време за context switch при coroutines?

A) 10-100 наносекунди  
B) 1-10 микросекунди  
C) 100-1000 микросекунди  
D) 1-10 милисекунди  

**Отговор:** A) 10-100 наносекунди

---

### Въпрос 18

Какво е ABA problem в lock-free програмирането?

A) Проблем с алокация на памет  
B) Стойност се променя от A на B и обратно на A, което заблуждава CAS операцията  
C) Проблем с именуване на променливи  
D) Грешка в компилатора  

**Отговор:** B) Стойност се променя от A на B и обратно на A, което заблуждава CAS операцията

---

### Въпрос 19

Кой метод на `std::coroutine_handle` се използва за продължаване на изпълнението на coroutine?

A) `continue()`  
B) `resume()`  
C) `start()`  
D) `run()`  

**Отговор:** B) `resume()`

---

### Въпрос 20

Какво връща `std::thread::hardware_concurrency()`?

A) Броя на активните нишки  
B) Броя на наличните CPU ядра  
C) Максималния брой нишки  
D) Текущото натоварване на CPU  

**Отговор:** B) Броя на наличните CPU ядра

---

### Въпрос 21

Кой от следните НЕ е компонент на Publisher/Subscriber pattern?

A) Publisher  
B) Subscriber  
C) Event Broker  
D) Mutex Manager  

**Отговор:** D) Mutex Manager

---

### Въпрос 22

Какво е предимството на `compare_exchange_weak` пред `compare_exchange_strong`?

A) По-силна гаранция за успех  
B) По-бързо на някои архитектури, но може да fail spuriously  
C) По-малко използване на памет  
D) По-добра четимост на кода  

**Отговор:** B) По-бързо на някои архитектури, но може да fail spuriously

---

### Въпрос 23

Къде се съхраняват локалните променливи на coroutine?

A) На stack-а на caller функцията  
B) В coroutine frame (heap)  
C) В глобална памет  
D) В регистрите на CPU  

**Отговор:** B) В coroutine frame (heap)

---

### Въпрос 24

Какво е основното предимство на Pub/Sub pattern пред директната комуникация?

A) По-бързо изпълнение  
B) Loose coupling между компоненти  
C) По-малко използване на памет  
D) По-лесно debugging  

**Отговор:** B) Loose coupling между компоненти

---

### Въпрос 25

Кой memory order се използва за "fire and forget" операции без нужда от синхронизация?

A) `memory_order_seq_cst`  
B) `memory_order_acquire`  
C) `memory_order_relaxed`  
D) `memory_order_release`  

**Отговор:** C) `memory_order_relaxed`

---

### Въпрос 26

Какво е предназначението на `std::suspend_always` в coroutine?

A) Никога не suspend-ва  
B) Винаги suspend-ва  
C) Suspend-ва при определени условия  
D) Унищожава coroutine  

**Отговор:** B) Винаги suspend-ва

---

### Въпрос 27

Кой от следните е валиден начин за решаване на backpressure?

A) Увеличаване на RAM  
B) Throttling на publisher-а  
C) Рестартиране на системата  
D) Игнориране на проблема  

**Отговор:** B) Throttling на publisher-а

---

### Въпрос 28

Какво е SPSC Queue?

A) Single Producer Single Consumer Queue  
B) Synchronized Parallel Sequential Container  
C) Shared Pointer Smart Container  
D) System Process Scheduling Controller  

**Отговор:** A) Single Producer Single Consumer Queue

---

### Въпрос 29

Коя библиотека предоставя `tbb::parallel_for`?

A) Boost  
B) Qt  
C) oneTBB (Threading Building Blocks)  
D) OpenMP  

**Отговор:** C) oneTBB (Threading Building Blocks)

---

### Въпрос 30

Какво е предимството на per-worker queues в Thread Pool?

A) По-лесна имплементация  
B) Намалява contention между нишките  
C) По-малко използване на памет  
D) По-добра съвместимост  

**Отговор:** B) Намалява contention между нишките

---

### Въпрос 31

Какво е "happens-before" relationship?

A) Времева последователност на събития  
B) Гаранция, че операция A е видима за операция B  
C) Приоритет на нишки  
D) Ред на компилация  

**Отговор:** B) Гаранция, че операция A е видима за операция B

---

### Въпрос 32

Кой метод на `promise_type` се извиква при `co_return value`?

A) `return_void()`  
B) `return_value()`  
C) `final_suspend()`  
D) `get_return_object()`  

**Отговор:** B) `return_value()`

---

### Въпрос 33

Какво е cache line alignment и защо е важно?

A) Подравняване на данни за по-бърз достъп и избягване на false sharing  
B) Оптимизация на компилатора  
C) Метод за криптиране  
D) Техника за компресия  

**Отговор:** A) Подравняване на данни за по-бърз достъп и избягване на false sharing

---

### Въпрос 34

Кой от следните е валиден awaitable в C++20?

A) `std::suspend_always`  
B) `std::thread`  
C) `std::mutex`  
D) `std::vector`  

**Отговор:** A) `std::suspend_always`

---

### Въпрос 35

Какво е "spinlock"?

A) Lock, който блокира нишката  
B) Lock, който използва busy-waiting  
C) Lock за файлова система  
D) Lock за мрежови операции  

**Отговор:** B) Lock, който използва busy-waiting

---

### Въпрос 36

Какво е предимството на асинхронната Pub/Sub имплементация с Thread Pool?

A) По-проста имплементация  
B) Non-blocking publish() и паралелна обработка  
C) По-малко използване на памет  
D) По-добра съвместимост с legacy код  

**Отговор:** B) Non-blocking publish() и паралелна обработка

---

### Въпрос 37

Кой метод на awaitable се извиква за да провери дали трябва да suspend-не?

A) `await_suspend()`  
B) `await_resume()`  
C) `await_ready()`  
D) `await_check()`  

**Отговор:** C) `await_ready()`

---

### Въпрос 38

Какво е "double-checked locking"?

A) Заключване два пъти за сигурност  
B) Pattern за lazy initialization с минимално заключване  
C) Метод за debugging  
D) Техника за криптиране  

**Отговор:** B) Pattern за lazy initialization с минимално заключване

---

### Въпрос 39

Кой от следните е правилен начин за capture на променлива в lambda за coroutine?

A) Capture by reference `[&x]`  
B) Capture by value `[x]`  
C) Capture всичко by reference `[&]`  
D) Не се използват lambda в coroutines  

**Отговор:** B) Capture by value `[x]` (за да се избегнат dangling references)

---

### Въпрос 40

Какво е "wait-free" алгоритъм?

A) Алгоритъм без изчакване на I/O  
B) Алгоритъм, при който всяка операция завършва в ограничен брой стъпки  
C) Алгоритъм без mutex  
D) Алгоритъм без threads  

**Отговор:** B) Алгоритъм, при който всяка операция завършва в ограничен брой стъпки

---

## Отворени въпроси

### Въпрос 41

Обяснете защо създаването на нова нишка за всяка задача е неефективно и какви проблеми може да причини.

**Отговор:**
Създаването на нова нишка за всяка задача е неефективно поради:

1. **System call overhead** - всяко създаване изисква обръщение към ОС
2. **Заделяне на stack памет** - всяка нишка заделя 1-2 MB stack памет
3. **Context switching overhead** - превключването между много нишки е скъпо
4. **Липса на контрол** - няма ограничение на броя едновременни нишки
5. **Cache invalidation** - честото създаване/унищожаване влошава cache locality
6. **Ресурсно изчерпване** - при много задачи може да се изчерпи паметта

---

### Въпрос 42

Опишете основните компоненти на Thread Pool и как работят заедно.

**Отговор:**
Основните компоненти на Thread Pool са:

1. **Worker threads** - фиксиран брой нишки, които обработват задачи
2. **Task queue** - споделена опашка със задачи за изпълнение
3. **Mutex** - защитава опашката от race conditions
4. **Condition variable** - събужда worker нишки когато има нови задачи
5. **Stop flag** - сигнализира на worker-ите да спрат

Работен процес: Worker нишките чакат на condition variable. Когато се добави задача, една нишка се събужда, взема задачата от опашката и я изпълнява.

---

### Въпрос 43

Какво е "contention" при lock-based структури и как lock-free структурите го решават?

**Отговор:**
**Contention** е ситуация, при която множество нишки се конкурират за един и същ ресурс (mutex). Това води до блокиране на нишки, context switching overhead и намалена throughput.

**Lock-free структурите** решават проблема чрез използване на атомарни операции (CAS) вместо mutex, non-blocking алгоритми и retry механизми при конфликт вместо изчакване.

---

### Въпрос 44

Обяснете разликата между `memory_order_acquire` и `memory_order_release`.

**Отговор:**

- **`memory_order_release`** - гарантира, че всички записи преди тази операция са видими за други нишки, които четат със `acquire`. Използва се при store операции.
- **`memory_order_acquire`** - гарантира, че всички четения след тази операция виждат записите направени преди съответния `release`. Използва се при load операции.

Заедно създават "happens-before" relationship между нишки.

---

### Въпрос 45

Защо coroutines са по-евтини от threads? Посочете поне 4 причини.

**Отговор:**

1. **Без system calls** - coroutines се управляват от програмата, не от ОС
2. **Малко състояние** - само локални променливи се съхраняват (< 1 KB vs 1-2 MB)
3. **Бърз context switch** - 10-100 ns vs 1-10 μs за threads
4. **Compiler оптимизации** - възможност за inline и други оптимизации
5. **Кооперативен scheduling** - без preemption overhead

---

### Въпрос 46

Какво е "dangling reference" проблемът при coroutines и как се избягва?

**Отговор:**
**Dangling reference** възниква когато coroutine държи референция/указател към данни, които вече не съществуват след suspend.

**Решения:**

1. Capture by value вместо by reference: `[x]` вместо `[&x]`
2. Използване на `std::shared_ptr` за споделени данни
3. `std::move` за преместване на данни в coroutine frame
4. Копиране на данни като параметри by value

---

### Въпрос 47

Опишете трите гаранции за доставка в Pub/Sub системи.

**Отговор:**

1. **At-most-once** - най-бързо, но може да загуби съобщения. За некритични данни.
2. **At-least-once** - гарантира доставка, но може да има дубликати. Изисква idempotent обработка.
3. **Exactly-once** - най-сложно, изисква транзакции. За критични системи.

---

### Въпрос 48

Какво е Event Loop и каква е ролята му при работа с coroutines?

**Отговор:**
**Event Loop** е механизъм за управление на coroutines:

- Поддържа опашка от готови за изпълнение coroutines
- Извиква `resume()` на coroutines когато са готови
- Управлява scheduling без OS threads
- Обработва I/O събития и събужда съответните coroutines

---

### Въпрос 49

Обяснете какво е "false sharing" и как се избягва.

**Отговор:**
**False sharing** възниква когато две нишки модифицират различни променливи в една и съща cache line. Това причинява ненужна cache invalidation.

**Решения:**

1. Cache line alignment - подравняване на данни на 64 байта
2. Padding - добавяне на допълнителни байтове между променливи
3. `alignas(64)` - C++ атрибут за подравняване

---

### Въпрос 50

Какви са предимствата и недостатъците на Observer pattern спрямо Publisher/Subscriber pattern?

**Отговор:**
**Observer:** Прост, директна връзка, синхронно изпълнение. Недостатъци: tight coupling, трудно scaling.

**Pub/Sub:** Loose coupling, асинхронност, scalability. Недостатъци: по-сложна имплементация, допълнителен overhead от broker.

---

### Въпрос 51

Опишете как работи Compare-And-Swap (CAS) операцията.

**Отговор:**
CAS е атомарна операция: атомарно прочита текущата стойност, сравнява с очакваната, ако са равни - записва новата стойност и връща true, иначе връща false без промяна.

---

### Въпрос 52

Какво е "promise_type" в C++ coroutines и какви методи трябва да съдържа?

**Отговор:**
**promise_type** контролира поведението на coroutine. Задължителни методи:

1. `get_return_object()` - създава return обекта
2. `initial_suspend()` - определя дали да suspend-не в началото
3. `final_suspend()` - поведение при завършване
4. `unhandled_exception()` - обработва exceptions
5. `return_value(T)` или `return_void()` - обработва co_return

---

### Въпрос 53

Обяснете разликата между blocking и non-blocking алгоритми.

**Отговор:**
**Blocking:** Нишка може да бъде спряна докато чака ресурс. Използват mutex. По-лесни за имплементация.

**Non-blocking:** Нишка никога не се блокира. Използват атомарни операции. По-добра scalability, но по-сложни.

---

### Въпрос 54

Какво е "ordering" проблемът в Pub/Sub системи и как се решава?

**Отговор:**
При паралелна обработка, съобщенията могат да бъдат обработени в различен ред от изпращането.

**Решения:** Single-threaded processing, partition-based ordering, sequence numbers, per-key ordering.

---

### Въпрос 55

Какво е "awaitable" в C++20 и какви методи трябва да има?

**Отговор:**
Awaitable е тип за `co_await`. Методи:

1. `await_ready()` - връща bool; ако true, не suspend-ва
2. `await_suspend(std::coroutine_handle<>)` - извиква се при suspend
3. `await_resume()` - връща резултата при resume

---

### Въпрос 56

Опишете hybrid подхода за комбиниране на Thread Pool, Coroutines и Pub/Sub.

**Отговор:**

1. **Thread Pool** - за CPU-intensive задачи
2. **Coroutines** - за I/O операции
3. **Pub/Sub** - за декуплиране и организация

Предимства: Използва силните страни на всяка техника.

---

### Въпрос 57

Какво е "coroutine frame" и какво съдържа?

**Отговор:**
Heap-allocated структура съдържаща: Promise object, локални променливи, resume point, параметри и временни обекти. Типично < 1 KB.

---

### Въпрос 58

Обяснете какво е "priority inversion".

**Отговор:**
Нископриоритетна нишка държи lock, високоприоритетна чака, средноприоритетна изпреварва ниската. Резултат: високоприоритетната ефективно има най-нисък приоритет.

---

### Въпрос 59

Какви са приложенията на Publisher/Subscriber pattern в реални системи?

**Отговор:**
GUI Applications, Game engines, Microservices (RabbitMQ, Kafka), Real-Time Data Processing, IoT sensor networks, Financial Trading Systems.

---

### Въпрос 60

Какво е `std::atomic_flag` и как се използва за spinlock?

**Отговор:**
Най-простият атомарен тип. `test_and_set()` атомарно задава true и връща предишната стойност. `clear()` задава false. За spinlock: spin докато `test_and_set()` връща true.

---

### Въпрос 61

Обяснете разликата между `co_await`, `co_yield` и `co_return`.

**Отговор:**

- `co_await` - suspend и чакане на резултат
- `co_yield` - връща междинна стойност без завършване
- `co_return` - завършва coroutine с финална стойност

---

### Въпрос 62

Какво е "thread-safe" и какви техники се използват за постигането му?

**Отговор:**
Код може да се изпълнява от множество нишки без race conditions. Техники: Mutex, atomic operations, lock-free структури, immutable данни, thread-local storage.

---

### Въпрос 63

Какво е oneTBB и какви паралелни алгоритми предоставя?

**Отговор:**
Библиотека от Intel за паралелно програмиране. Алгоритми: `parallel_for`, `parallel_for_each`, `parallel_reduce`, `task_group`.

---

### Въпрос 64

Обяснете какво е "work-stealing" scheduler.

**Отговор:**
Всеки worker има собствена опашка. Когато е празна, worker "краде" задачи от други workers. Предимства: автоматично балансиране, намалена contention, добра cache locality.

---

### Въпрос 65

Какво е "memory barrier" и защо е необходим?

**Отговор:**
Инструкция гарантираща определен ред на memory операции. Необходим заради compiler и CPU reordering, и cache coherency между ядра.

---

### Въпрос 66

Какво е "lazy initialization" и как се имплементира thread-safe?

**Отговор:**
Обект се създава при първото използване. Thread-safe: double-checked locking с atomics или C++11 static local (гарантирано thread-safe по стандарт).

---

### Въпрос 67

Обяснете разликата между синхронна и асинхронна Pub/Sub имплементация.

**Отговор:**
**Синхронна:** `publish()` блокира докато всички обработят. **Асинхронна:** `publish()` връща веднага, паралелна обработка чрез Thread Pool.

---

### Въпрос 68

Какво е "idempotent" операция и защо е важна при at-least-once доставка?

**Отговор:**
Операция с еднакъв резултат при многократно изпълнение. Важна защото при at-least-once може да има дубликати - idempotent операции не причиняват проблеми.

---

### Въпрос 69

Какво е "bounded queue" и защо е важна при backpressure?

**Отговор:**
Опашка с фиксиран капацитет. Предотвратява memory exhaustion, сигнализира за backpressure и принуждава throttling.

---

### Въпрос 70

Обяснете какво е "cache locality" и защо Thread Pool подобрява cache locality.

**Отговор:**
Достъпът до близки memory адреси е по-бърз заради CPU cache. Thread Pool подобрява чрез преизползване на нишки (данните остават в cache), намален context switch и per-worker queues.

---

## Въпроси с код

### Въпрос 71

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
        { std::lock_guard<std::mutex> lock(queue_mutex); tasks.push(std::move(task)); }
        condition.notify_one();
    }

    ~ThreadPool() {
        { std::lock_guard<std::mutex> lock(queue_mutex); stop = true; }
        condition.notify_all();
        for (auto& w : workers) w.join();
    }
};
```

---

### Въпрос 72

Напишете lock-free counter използвайки `std::atomic` и `compare_exchange_weak`.

**Отговор:**

```cpp
#include <atomic>

class LockFreeCounter {
    std::atomic<int> count{0};

public:
    void increment() {
        int expected = count.load(std::memory_order_relaxed);
        while (!count.compare_exchange_weak(expected, expected + 1,
            std::memory_order_release, std::memory_order_relaxed)) {}
    }

    int get() const { return count.load(std::memory_order_acquire); }
};
```

---

### Въпрос 73

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

---

### Въпрос 74

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

---

### Въпрос 75

Напишете spinlock използвайки `std::atomic_flag`.

**Отговор:**

```cpp
#include <atomic>
#include <thread>

class Spinlock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();
    }
    void unlock() { flag.clear(std::memory_order_release); }
};
```

---

### Въпрос 76

Напишете custom awaitable, който suspend-ва за поне определен брой милисекунди.

**Отговор:**

```cpp
#include <coroutine>
#include <chrono>
#include <thread>

struct SleepAwaitable {
    std::chrono::milliseconds duration;
    bool await_ready() const noexcept { return duration.count() <= 0; }
    void await_suspend(std::coroutine_handle<> h) const {
        std::thread([h, d = duration]() {
            std::this_thread::sleep_for(d);
            h.resume();
        }).detach();
    }
    void await_resume() const noexcept {}
};

Task do_something()
{
    co_await SleepAwaitable{50ms};
}

int main()
{
    auto action = do_something();
    action.resume();
    return 0;
}
```

---

### Въпрос 77

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

---

### Въпрос 78

Напишете thread-safe singleton използвайки double-checked locking.

**Отговор:**

```cpp
#include <atomic>
#include <mutex>

class Singleton {
    static std::atomic<Singleton*> instance;
    static std::mutex mutex;
    Singleton() {}
public:
    static Singleton* get_instance() {
        Singleton* tmp = instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            tmp = instance.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new Singleton();
                instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }
};
std::atomic<Singleton*> Singleton::instance{nullptr};
std::mutex Singleton::mutex;
```

---

### Въпрос 79

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

---

### Въпрос 80

Напишете generator coroutine използвайки `co_yield`.

**Отговор:**

```cpp
#include <coroutine>

template<typename T>
struct Generator {
    struct promise_type {
        T current_value;
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) { current_value = value; return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    std::coroutine_handle<promise_type> handle;
    bool next() { handle.resume(); return !handle.done(); }
    T value() { return handle.promise().current_value; }
    ~Generator() { if (handle) handle.destroy(); }
};

Generator<int> range(int start, int end) {
    for (int i = start; i <= end; ++i) co_yield i;
}
```

---

### Въпрос 81

Напишете RAII wrapper за `std::mutex`.

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

---

### Въпрос 82

Напишете паралелен for цикъл използвайки threads.

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

---

### Въпрос 83

Напишете bounded queue с фиксиран капацитет.

**Отговор:**

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T, size_t MaxSize = 100>
class BoundedQueue {
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_full_, not_empty_;

public:
    void push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return queue_.size() < MaxSize; });
        queue_.push(std::move(item));
        not_empty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }
};
```

---

### Въпрос 84

Напишете функция за паралелно сумиране на масив.

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

---

### Въпрос 85

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

---

### Въпрос 86

Напишете функция, която изпълнява задача с timeout.

**Отговор:**

```cpp
#include <thread>
#include <future>
#include <chrono>
#include <optional>

template<typename Func>
std::optional<decltype(std::declval<Func>()())> 
run_with_timeout(Func f, std::chrono::milliseconds timeout) {
    auto future = std::async(std::launch::async, f);
    if (future.wait_for(timeout) == std::future_status::ready) {
        return future.get();
    }
    return std::nullopt;
}
```

---

### Въпрос 87

Напишете read-write lock използвайки `std::shared_mutex`.

**Отговор:**

```cpp
#include <shared_mutex>
#include <vector>

class ThreadSafeVector {
    std::vector<int> data_;
    mutable std::shared_mutex mutex_;

public:
    void push(int value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_.push_back(value);
    }

    int get(size_t index) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.at(index);
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.size();
    }
};
```

---

### Въпрос 88

Напишете async EventBroker с Thread Pool.

**Отговор:**

```cpp
#include <functional>
#include <vector>
#include <mutex>

// Предполагаме ThreadPool от въпрос 71

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

---

### Въпрос 89

Напишете coroutine Task с exception handling.

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

---

### Въпрос 90

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

---

### Въпрос 91

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

---

### Въпрос 92

Напишете semaphore използвайки mutex и condition variable.

**Отговор:**

```cpp
#include <mutex>
#include <condition_variable>

class Semaphore {
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;

public:
    explicit Semaphore(int initial = 0) : count_(initial) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }

    void release() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }
};
```

---

### Въпрос 93

Напишете barrier за синхронизация на нишки.

**Отговор:**

```cpp
#include <mutex>
#include <condition_variable>

class Barrier {
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
    int waiting_ = 0;
    int generation_ = 0;

public:
    explicit Barrier(int count) : count_(count) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;
        if (++waiting_ == count_) {
            ++generation_;
            waiting_ = 0;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }
};
```

---

### Въпрос 94

Напишете thread pool с futures за получаване на резултати.

**Отговор:**

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>

class ThreadPoolWithFutures {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;

public:
    ThreadPoolWithFutures(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<typename F>
    auto enqueue(F&& f) -> std::future<decltype(f())> {
        auto task = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));
        auto future = task->get_future();
        { std::lock_guard<std::mutex> lock(mtx); tasks.emplace([task]() { (*task)(); }); }
        cv.notify_one();
        return future;
    }

    ~ThreadPoolWithFutures() {
        { std::lock_guard<std::mutex> lock(mtx); stop = true; }
        cv.notify_all();
        for (auto& w : workers) w.join();
    }
};
```

---

### Въпрос 95

Напишете lock-free stack.

**Отговор:**

```cpp
#include <atomic>
#include <memory>

template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(std::move(d)), next(nullptr) {}
    };
    std::atomic<Node*> head{nullptr};

public:
    void push(T data) {
        Node* new_node = new Node(std::move(data));
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node->next, new_node,
            std::memory_order_release, std::memory_order_relaxed)) {}
    }

    std::shared_ptr<T> pop() {
        Node* old_head = head.load(std::memory_order_acquire);
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next,
            std::memory_order_release, std::memory_order_acquire)) {}
        if (!old_head) return nullptr;
        auto result = std::make_shared<T>(std::move(old_head->data));
        delete old_head;
        return result;
    }
};
```

---

### Въпрос 96

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

---

### Въпрос 97

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

---

### Въпрос 98

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

---

### Въпрос 99

Напишете thread-safe lazy initialization с `std::call_once`.

**Отговор:**

```cpp
#include <mutex>
#include <memory>

class ExpensiveResource {
public:
    ExpensiveResource() { /* expensive initialization */ }
    void use() { /* ... */ }
};

class ResourceManager {
    std::unique_ptr<ExpensiveResource> resource;
    std::once_flag init_flag;

public:
    ExpensiveResource& get() {
        std::call_once(init_flag, [this]() {
            resource = std::make_unique<ExpensiveResource>();
        });
        return *resource;
    }
};
```

---

### Въпрос 100

Напишете hybrid система комбинираща Thread Pool и Pub/Sub.

**Отговор:**

```cpp
#include <functional>
#include <vector>
#include <mutex>
// Предполагаме ThreadPool от въпрос 71

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

---
