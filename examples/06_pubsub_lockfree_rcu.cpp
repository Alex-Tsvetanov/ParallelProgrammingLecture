// Example 6: Lock-free Publisher/Subscriber with RCU
// Demonstrates Read-Copy-Update pattern for lock-free subscriber list
// Topics: std::atomic, std::shared_ptr, memory ordering, RCU pattern

#include <atomic>
#include <memory>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

template<typename Event>
class RCUEventBroker {
public:
    using Callback = std::function<void(const Event&)>;

private:
    struct SubscriberNode {
        Callback callback;
        std::shared_ptr<SubscriberNode> next;
        
        SubscriberNode(Callback cb) : callback(std::move(cb)), next(nullptr) {}
    };

    // Head of the subscriber linked list
    std::atomic<std::shared_ptr<SubscriberNode>> head;

public:
    RCUEventBroker() : head(nullptr) {}

    // Lock-free subscription using Compare-And-Swap
    void subscribe(Callback callback) {
        auto new_node = std::make_shared<SubscriberNode>(std::move(callback));
        
        // Load current head
        auto old_head = head.load(std::memory_order_acquire);
        
        do {
            // Link new node to current head
            new_node->next = old_head;
            // Try to make new_node the new head
        } while (!head.compare_exchange_weak(
            old_head, new_node,
            std::memory_order_release,
            std::memory_order_acquire));
        
        std::cout << "[RCU Broker] Subscriber added (lock-free)\n";
    }

    // Wait-free publish (read-only operation)
    void publish(const Event& event) {
        // Lock-free read of subscriber list
        auto node = head.load(std::memory_order_acquire);
        
        int count = 0;
        while (node) {
            // Call each subscriber's callback
            node->callback(event);
            node = node->next;
            count++;
        }
        
        std::cout << "[RCU Broker] Published to " << count 
                  << " subscribers (wait-free)\n";
    }

    // Count subscribers (for demonstration)
    size_t count_subscribers() const {
        auto node = head.load(std::memory_order_acquire);
        size_t count = 0;
        while (node) {
            count++;
            node = node->next;
        }
        return count;
    }
};

// Example event
struct SensorReading {
    int sensor_id;
    double value;
    long timestamp;
};

// Test concurrent subscriptions and publishes
void subscriber_thread(RCUEventBroker<SensorReading>& broker, int id, int count) {
    for (int i = 0; i < count; ++i) {
        broker.subscribe([id, i](const SensorReading& reading) {
            std::cout << "  [Subscriber " << id << "-" << i << "] "
                     << "Sensor " << reading.sensor_id << " = " 
                     << reading.value << "\n";
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void publisher_thread(RCUEventBroker<SensorReading>& broker, int id, int count) {
    for (int i = 0; i < count; ++i) {
        SensorReading reading{id, 20.0 + i * 0.5, 1000000 + i};
        std::cout << "[Publisher " << id << "] Publishing reading " << i << "\n";
        broker.publish(reading);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    std::cout << "=== Lock-Free Publisher/Subscriber with RCU ===\n\n";

    RCUEventBroker<SensorReading> broker;

    std::cout << "--- Phase 1: Adding initial subscribers ---\n";
    std::vector<std::thread> threads;

    // Add subscribers concurrently
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(subscriber_thread, std::ref(broker), i, 2);
    }

    for (auto& t : threads) {
        t.join();
    }
    threads.clear();

    std::cout << "\nInitial subscriber count: " << broker.count_subscribers() << "\n\n";

    std::cout << "--- Phase 2: Concurrent publishing and subscribing ---\n";
    
    // Start publishers
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(publisher_thread, std::ref(broker), i, 3);
    }

    // Add more subscribers while publishing
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    for (int i = 3; i < 5; ++i) {
        threads.emplace_back(subscriber_thread, std::ref(broker), i, 2);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nFinal subscriber count: " << broker.count_subscribers() << "\n";

    std::cout << "\n--- Phase 3: Final publish to all subscribers ---\n";
    broker.publish({99, 100.0, 9999999});

    std::cout << "\nDemo complete. Notice:\n";
    std::cout << "  1. No mutexes used for subscribe or publish\n";
    std::cout << "  2. Subscribers can be added while publishing\n";
    std::cout << "  3. Publishing is wait-free (never blocks)\n";

    return 0;
}
