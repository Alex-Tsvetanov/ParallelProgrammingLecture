// Example 2: Lock-free Queue using std::atomic
// Demonstrates Compare-And-Swap (CAS) operations and memory ordering
// Topics: std::atomic, compare_exchange_weak, memory_order

#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;
        
        Node() : next(nullptr) {}
        explicit Node(T value) 
            : data(std::make_shared<T>(std::move(value))), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        while (Node* old_head = head.load(std::memory_order_relaxed)) {
            head.store(old_head->next.load(std::memory_order_relaxed), 
                      std::memory_order_relaxed);
            delete old_head;
        }
    }

    void enqueue(T value) {
        Node* new_node = new Node(std::move(value));
        Node* old_tail = tail.load(std::memory_order_acquire);
        
        // Try to set tail->next to new_node
        while (true) {
            Node* null_ptr = nullptr;
            if (old_tail->next.compare_exchange_weak(
                null_ptr, new_node,
                std::memory_order_release,
                std::memory_order_acquire)) {
                break;
            }
            // Someone else added a node, swing tail forward
            old_tail = old_tail->next.load(std::memory_order_acquire);
        }
        
        // Try to swing tail to new_node
        tail.compare_exchange_strong(old_tail, new_node,
                                     std::memory_order_release,
                                     std::memory_order_acquire);
    }

    std::shared_ptr<T> dequeue() {
        Node* old_head = head.load(std::memory_order_acquire);
        
        while (true) {
            Node* next = old_head->next.load(std::memory_order_acquire);
            if (next == nullptr) {
                return nullptr; // Queue is empty
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

    bool empty() const {
        Node* h = head.load(std::memory_order_acquire);
        Node* n = h->next.load(std::memory_order_acquire);
        return n == nullptr;
    }
};

// Test the lock-free queue
void producer(LockFreeQueue<int>& queue, int id, int count) {
    for (int i = 0; i < count; ++i) {
        int value = id * 1000 + i;
        queue.enqueue(value);
        {
            std::stringstream ss;
            ss << "Producer " << id << " enqueued: " << value << "\n";
            std::cout << ss.str() << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void consumer(LockFreeQueue<int>& queue, int id, int expected_count) {
    int consumed = 0;
    while (consumed < expected_count) {
        auto value = queue.dequeue();
        if (value) {
            {
                std::stringstream ss;
                ss << "Consumer " << id << " dequeued: " << *value << "\n";
                std::cout << ss.str() << std::flush;
            }
            consumed++;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

int main() {
    {
        std::stringstream ss;
        ss << "=== Lock-Free Queue Example ===\n\n";
        std::cout << ss.str() << std::flush;
    }
    
    LockFreeQueue<int> queue;
    
    const int num_producers = 3;
    const int num_consumers = 2;
    const int items_per_producer = 5;
    
    std::vector<std::thread> threads;
    
    // Start producers
    for (int i = 0; i < num_producers; ++i) {
        threads.emplace_back(producer, std::ref(queue), i, items_per_producer);
    }
    
    // Start consumers
    int items_per_consumer = (num_producers * items_per_producer) / num_consumers;
    for (int i = 0; i < num_consumers; ++i) {
        threads.emplace_back(consumer, std::ref(queue), i, items_per_consumer);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    {
        std::stringstream ss;
        ss << "\nAll threads completed\n";
        ss << "Queue empty: " << (queue.empty() ? "yes" : "no") << "\n";
        std::cout << ss.str() << std::flush;
    }
    
    return 0;
}
