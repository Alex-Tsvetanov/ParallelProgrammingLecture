// Example 4: Publisher/Subscriber Pattern (Synchronous)
// Demonstrates event-driven architecture with callbacks
// Topics: std::function, std::vector, design patterns

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>

// Event types
struct TemperatureEvent {
    double temperature;
    std::string sensor_id;
};

struct HumidityEvent {
    double humidity;
    std::string sensor_id;
};

// Generic Event Broker
template<typename Event>
class EventBroker {
public:
    using Callback = std::function<void(const Event&)>;
    using SubscriptionId = size_t;

private:
    struct Subscription {
        SubscriptionId id;
        Callback callback;
    };

    std::vector<Subscription> subscribers;
    SubscriptionId next_id = 0;

public:
    SubscriptionId subscribe(Callback callback) {
        SubscriptionId id = next_id++;
        subscribers.push_back({id, std::move(callback)});
        std::cout << "[Broker] Subscriber " << id << " registered\n";
        return id;
    }

    void unsubscribe(SubscriptionId id) {
        auto it = std::remove_if(subscribers.begin(), subscribers.end(),
            [id](const Subscription& sub) { return sub.id == id; });
        
        if (it != subscribers.end()) {
            subscribers.erase(it, subscribers.end());
            std::cout << "[Broker] Subscriber " << id << " unregistered\n";
        }
    }

    void publish(const Event& event) {
        std::cout << "[Broker] Publishing event to " << subscribers.size() 
                  << " subscribers\n";
        for (const auto& sub : subscribers) {
            sub.callback(event);
        }
    }

    size_t subscriber_count() const {
        return subscribers.size();
    }
};

// Example subscribers
class HeatingSystem {
public:
    void on_temperature(const TemperatureEvent& event) {
        std::cout << "  [HeatingSystem] Temperature from " << event.sensor_id 
                  << ": " << event.temperature << "°C";
        if (event.temperature < 18.0) {
            std::cout << " - HEATING ON";
        } else if (event.temperature > 24.0) {
            std::cout << " - HEATING OFF";
        }
        std::cout << "\n";
    }
};

class MobileApp {
public:
    void on_temperature(const TemperatureEvent& event) {
        std::cout << "  [MobileApp] Notification: " << event.sensor_id 
                  << " reports " << event.temperature << "°C\n";
    }

    void on_humidity(const HumidityEvent& event) {
        std::cout << "  [MobileApp] Notification: " << event.sensor_id 
                  << " reports " << event.humidity << "% humidity\n";
    }
};

class DataLogger {
public:
    void log_temperature(const TemperatureEvent& event) {
        std::cout << "  [DataLogger] LOG: temp," << event.sensor_id << "," 
                  << event.temperature << "\n";
    }

    void log_humidity(const HumidityEvent& event) {
        std::cout << "  [DataLogger] LOG: humidity," << event.sensor_id << "," 
                  << event.humidity << "\n";
    }
};

int main() {
    std::cout << "=== Publisher/Subscriber Pattern Example ===\n\n";

    // Create event brokers
    EventBroker<TemperatureEvent> temp_broker;
    EventBroker<HumidityEvent> humidity_broker;

    // Create subscribers
    HeatingSystem heating;
    MobileApp app;
    DataLogger logger;

    std::cout << "--- Registering Subscribers ---\n";
    
    // Subscribe to temperature events
    auto heating_sub = temp_broker.subscribe(
        [&heating](const TemperatureEvent& e) { heating.on_temperature(e); });
    
    auto app_temp_sub = temp_broker.subscribe(
        [&app](const TemperatureEvent& e) { app.on_temperature(e); });
    
    auto logger_temp_sub = temp_broker.subscribe(
        [&logger](const TemperatureEvent& e) { logger.log_temperature(e); });

    // Subscribe to humidity events
    auto app_humidity_sub = humidity_broker.subscribe(
        [&app](const HumidityEvent& e) { app.on_humidity(e); });
    
    auto logger_humidity_sub = humidity_broker.subscribe(
        [&logger](const HumidityEvent& e) { logger.log_humidity(e); });

    std::cout << "\n--- Publishing Events ---\n";
    
    // Publish temperature events
    std::cout << "\n1. Temperature event (cold):\n";
    temp_broker.publish({15.5, "sensor-living-room"});

    std::cout << "\n2. Temperature event (hot):\n";
    temp_broker.publish({26.0, "sensor-bedroom"});

    std::cout << "\n3. Humidity event:\n";
    humidity_broker.publish({65.0, "sensor-bathroom"});

    std::cout << "\n--- Unsubscribing Heating System ---\n";
    temp_broker.unsubscribe(heating_sub);

    std::cout << "\n4. Temperature event (heating unsubscribed):\n";
    temp_broker.publish({20.0, "sensor-kitchen"});

    std::cout << "\nFinal subscriber counts:\n";
    std::cout << "  Temperature: " << temp_broker.subscriber_count() << "\n";
    std::cout << "  Humidity: " << humidity_broker.subscriber_count() << "\n";

    return 0;
}
