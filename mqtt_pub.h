#include <mosquitto.h>

class mqtt {
public:
    mqtt() : mosq(nullptr) {
        mosquitto_lib_init();  // Initialize the library
        mosq = mosquitto_new("publisher-test", true, nullptr);
        if (mosq == nullptr) {
            std::cerr << "Failed to create mosquitto client instance.\n";
        }
    }

    ~mqtt() {
        if (mosq != nullptr) {
            mosquitto_disconnect(mosq);
            mosquitto_destroy(mosq);
            mosq = nullptr;
        }
        mosquitto_lib_cleanup();
    }
/*
    void init() {
5yy        if (mosq == nullptr) {
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
        if (mosq == nullptr) {
            std::cerr << "Mosquitto client not initialized.\n";
            return;
        }
        int rc = mosquitto_connect(mosq, "localhost", 1990, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Client could not connect to broker! Error Code: " << rc << "\n";
            mosquitto_destroy(mosq);
            mosq = nullptr;
            return;
        } else {
            std::cout << "We are now connected to the broker!\n";
        }
    }

    //void publish(const std::string& msg, const std::string& topic, int len) {
    void publish(const std::string &topic, const void *msg, int len) {
        if (mosq == nullptr) {
            std::cerr << "FATAL: Seems 'mosquitto' client is not initialized.\n";
            std::cerr << "Ensure 'mosquitto' broker is running and on right port number before publishing data\n";
            return;
                }

        //int rc = mosquitto_publish(mosq, NULL, "test/t1", 6, "Hello", 0, false);
        //mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);
        //int rc = mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);

        std::cout << "publishing msg: \"" << static_cast<const char*>(msg) << "\" of size " << len << std::endl;
        std::cout << "Z1\n";
        std::cout << mosq << ": " << "len=" << len << std::endl;
        std::cout << mosq << ": " << "msg=" << msg << std::endl;
        int rc = mosquitto_publish(mosq, NULL, topic.c_str(), len, msg, 0, false);
        std::cout << "Z2\n";
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Failed to publish message. Error Code: " << rc << "\n";
        }
    }

private:
    struct mosquitto *mosq;
};
