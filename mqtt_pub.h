#include <mosquitto.h>
#include <cstring>

// Function to print the error message based on system error (errno)
void handle_errno() {
    char buf[1024];  // Buffer to hold the error message
    strerror_r(errno, buf, sizeof(buf));  // Get the error message for current errno
    std::cout << "System Error: " << buf << " (errno: " << errno << ")\n";
}

// Function to print the error message based on Mosquitto return codes
void print_mqtt_error(int error_code) {
    switch (error_code) {
        case MOSQ_ERR_AUTH_CONTINUE:
            std::cout << "Mosquitto: Authentication continues.\n";
            break;
        case MOSQ_ERR_NO_SUBSCRIBERS:
            std::cout << "Mosquitto: No subscribers to the message.\n";
            break;
        case MOSQ_ERR_SUB_EXISTS:
            std::cout << "Mosquitto: Subscription already exists.\n";
            break;
        case MOSQ_ERR_CONN_PENDING:
            std::cout << "Mosquitto: Connection is pending.\n";
            break;
        case MOSQ_ERR_SUCCESS:
            std::cout << "Mosquitto: Success: Operation completed successfully.\n";
            break;
        case MOSQ_ERR_NOMEM:
            std::cout << "Mosquitto: Out of memory when allocating resources.\n";
            break;
        case MOSQ_ERR_PROTOCOL:
            std::cout << "Mosquitto: Protocol error detected.\n";
            break;
        case MOSQ_ERR_INVAL:
            std::cout << "Mosquitto: Invalid input parameter.\n";
            break;
        case MOSQ_ERR_NO_CONN:
            std::cout << "Mosquitto: No connection to broker.\n";
            break;
        case MOSQ_ERR_CONN_REFUSED:
            std::cout << "Mosquitto: Connection refused by the broker.\n";
            break;
        case MOSQ_ERR_NOT_FOUND:
            std::cout << "Mosquitto: Requested topic or message not found.\n";
            break;
        case MOSQ_ERR_CONN_LOST:
            std::cout << "Mosquitto: Connection to the broker was lost.\n";
            break;
        case MOSQ_ERR_TLS:
            std::cout << "Mosquitto: Problem with SSL/TLS connection.\n";
            break;
        case MOSQ_ERR_PAYLOAD_SIZE:
            std::cout << "Mosquitto: Payload too large.\n";
            break;
        case MOSQ_ERR_NOT_SUPPORTED:
            std::cout << "Mosquitto: Operation not supported by the broker.\n";
            break;
        case MOSQ_ERR_AUTH:
            std::cout << "Mosquitto: Authentication failed.\n";
            break;
        case MOSQ_ERR_ACL_DENIED:
            std::cout << "Mosquitto: Access control denied the operation.\n";
            break;
        case MOSQ_ERR_UNKNOWN:
            std::cout << "Mosquitto: Unknown error occurred.\n";
            break;
        case MOSQ_ERR_ERRNO:
            handle_errno();
            break;
        case MOSQ_ERR_EAI:
            std::cout << "Mosquitto: Error in address resolution.\n";
            break;
        case MOSQ_ERR_PROXY:
            std::cout << "Mosquitto: Proxy error.\n";
            break;
        case MOSQ_ERR_PLUGIN_DEFER:
            std::cout << "Mosquitto: Plugin deferred the operation.\n";
            break;
        case MOSQ_ERR_MALFORMED_UTF8:
            std::cout << "Mosquitto: Malformed UTF-8 string detected.\n";
            break;
        case MOSQ_ERR_KEEPALIVE:
            std::cout << "Mosquitto: Keepalive timeout.\n";
            break;
        case MOSQ_ERR_LOOKUP:
            std::cout << "Mosquitto: Error in DNS lookup.\n";
            break;
        case MOSQ_ERR_MALFORMED_PACKET:
            std::cout << "Mosquitto: Malformed MQTT packet.\n";
            break;
        case MOSQ_ERR_DUPLICATE_PROPERTY:
            std::cout << "Mosquitto: Duplicate property in the packet.\n";
            break;
        case MOSQ_ERR_TLS_HANDSHAKE:
            std::cout << "Mosquitto: TLS handshake failed.\n";
            break;
        case MOSQ_ERR_QOS_NOT_SUPPORTED:
            std::cout << "Mosquitto: QoS level not supported by the broker.\n";
            break;
        case MOSQ_ERR_OVERSIZE_PACKET:
            std::cout << "Mosquitto: Packet exceeds maximum allowed size.\n";
            break;
        case MOSQ_ERR_OCSP:
            std::cout << "Mosquitto: OCSP (Online Certificate Status Protocol) check failed.\n";
            break;
        case MOSQ_ERR_TIMEOUT:
            std::cout << "Mosquitto: Timeout occurred.\n";
            break;
        case MOSQ_ERR_RETAIN_NOT_SUPPORTED:
            std::cout << "Mosquitto: Retained messages are not supported.\n";
            break;
        case MOSQ_ERR_TOPIC_ALIAS_INVALID:
            std::cout << "Mosquitto: Invalid topic alias received.\n";
            break;
        case MOSQ_ERR_ADMINISTRATIVE_ACTION:
            std::cout << "Mosquitto: Administrative action is required.\n";
            break;
        case MOSQ_ERR_ALREADY_EXISTS:
            std::cout << "Mosquitto: Resource already exists.\n";
            break;
        default:
            std::cout << "Mosquitto: Unknown error code " << error_code << ".\n";
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

        std::cout << "publishing msg: " << static_cast<const char*>(msg) << " of size " << len << std::endl;
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
