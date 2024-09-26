#include <mosquitto.h>

// Function to print the error message based on MQTT return codes
void print_mqtt_error(int error_code) {
    switch (error_code) {
        case MQTTASYNC_SUCCESS:
            std::cout << "Success: Operation completed successfully.\n";
            break;
        case MQTTASYNC_FAILURE:
            std::cout << "Failure: Unspecified error occurred.\n";
            break;
        case MQTTASYNC_PERSISTENCE_ERROR:
            std::cout << "Persistence Error: Problem with file system persistence.\n";
            break;
        case MQTTASYNC_DISCONNECTED:
            std::cout << "Disconnected: Client is disconnected from the broker.\n";
            break;
        case MQTTASYNC_BAD_STRUCTURE:
            std::cout << "Bad Structure: The structure passed was invalid.\n";
            break;
        case MQTTASYNC_BAD_UTF8_STRING:
            std::cout << "Bad UTF-8 String: The topic or payload string was not valid UTF-8.\n";
            break;
        case MQTTASYNC_NULL_PARAMETER:
            std::cout << "Null Parameter: A required parameter was passed as null.\n";
            break;
        case MQTTASYNC_BAD_QOS:
            std::cout << "Bad QoS: Invalid Quality of Service level.\n";
            break;
        case MQTTASYNC_NO_MORE_MSGIDS:
            std::cout << "No More Message IDs: No message IDs available.\n";
            break;
        case MQTTASYNC_OPERATION_INCOMPLETE:
            std::cout << "Operation Incomplete: Operation has not completed yet.\n";
            break;
        case MQTTASYNC_MAX_BUFFERED_MESSAGES:
            std::cout << "Max Buffered Messages Reached: Too many buffered messages.\n";
            break;
        case MQTTASYNC_SSL_ERROR:
            std::cout << "SSL Error: A problem occurred with the SSL/TLS connection.\n";
            break;
        case MQTTASYNC_BAD_PROTOCOL:
            std::cout << "Bad Protocol: The requested protocol is not supported.\n";
            break;
        case MQTTASYNC_BAD_MQTT_VERSION:
            std::cout << "Bad MQTT Version: The requested MQTT version is not supported.\n";
            break;
        default:
            std::cout << "Unknown Error: Error code " << error_code << ".\n";
            break;
    }
}

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
            mosq = nullptr;
        } else {
            std::cout << "We are now connected to the broker!\n";
        }
    }

    //void publish(const std::string& msg, const std::string& topic, int len) {
    int publish(const void *msg, int len) {
        if (mosq == nullptr) {
            std::cerr << "FATAL: Seems 'mosquitto' client is not initialized.";
            std::cerr << "Ensure 'mosquitto' broker is running and on right port number before publishing data\n";
            return 255;  // Exit immediately with status 255, no point running;
        }

        //int rc = mosquitto_publish(mosq, NULL, "test/t1", 6, "Hello", 0, false);
        //mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);
        //int rc = mosquitto_publish(mosq, nullptr, topic.c_str(), len, msg.c_str(), 0, false);

        std::cout << "publishing msg: " << static_cast<const char*>(msg) <<  of size " << len << std::endl;
        std::cout << "Z1" << mosq << len << msg;
        int rc = mosquitto_publish(mosq, NULL, "test/t1", len, msg, 0, false);
        std::cout << "Z2";
        if (rc != MOSQ_ERR_SUCCESS) {
            print_mqtt_error(rc);
            //std::cerr << " Failed to publish message. Error Code: " << rc << "\n";
        }
        return rc;
    }

private:
    struct mosquitto *mosq;
};
