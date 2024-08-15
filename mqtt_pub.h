#include <mosquitto.h>

class mqtt {
public:
    mqtt() : mosq(nullptr) {
        mosquitto_lib_init();  // Initialize the library
        mosq = mosquitto_new("publisher-test", true, nullptr);
        if (mosq == nullptr) {
            std::cerr << "Failed to create mosquitto client instance.\n";
            // Handle error as appropriate
        }
    }

    ~mqtt() {
        if (mosq != nullptr) {
            mosquitto_disconnect(mosq);
            mosquitto_destroy(mosq);
        }
        mosquitto_lib_cleanup();
    }
/*
    void init() {
        if (mosq == nullptr) {
            std::cerr << "Mosquitto client not initialized.\n";
            return;
        }

        int rc = mosquitto_connect(mosq, "localhost", 1990, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Client could not connect to broker! Error Code: " << rc << "\n";
            mosquitto_destroy(mosq);
            mosq = nullptr;
        } else {
            std::cout << "We are now connected to the broker!\n";
        }
    }
*/
    void init(){
        //mosquitto_lib_init();
        //mosq = mosquitto_new("publisher-test", true, NULL);
        int rc = mosquitto_connect(mosq, "localhost", 1990, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Client could not connect to broker! Error Code: " << rc << "\n";
            mosquitto_destroy(mosq);
        } else {
            std::cout << "We are now connected to the broker!\n";
        }
    }

    //void publish(const std::string& msg, const std::string& topic, int len) {
    void publish(const void *msg, int len) {
        if (mosq == nullptr) {
            std::cerr << "Mosquitto client not initialized.\n";
            return;
        }

        //int rc = mosquitto_publish(mosq, NULL, "test/t1", 6, "Hello", 0, false);
        //mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);
        //int rc = mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);

                std::cout << "publishing msg: " << static_cast<const char*>(msg) << "of size " << len << std::endl;
                int rc = mosquitto_publish(mosq, NULL, "test/t1", len, msg, 0, false);
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Failed to publish message. Error Code: " << rc << "\n";
        }
    }

private:
    struct mosquitto *mosq;
};
