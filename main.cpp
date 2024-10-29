#include <iostream>
#include <string>
#include <vector>
#include <mosquitto.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// MQTT parameters
const char* SERVER_ADDRESS = "localhost";
const int SERVER_PORT = 1883;
const char* CLIENT_ID = "NaviMQTTTranslatorClient";
const char* TOPIC_REQUEST_COMM = "diagstack/request/opencommchannel";
const char* TOPIC_RESPONSE_COMM = "diagstack/response/opencommchannel";
const char* TOPIC_READ_PGN_REQUEST = "diagstack/request/readpgns";
const char* TOPIC_READ_PGN_RESPONSE = "diagstack/response/readpgns";

// Supported PGNs
const std::vector<std::string> SUPPORTED_PGNS = {
    "F001", "FD9F", "FDA3", "FE9A", "FEA4", "FEEE", "FEF0", "FEF5", "FEF6", "FEF7"
};

class Translator {
public:
    Translator() {
        mosquitto_lib_init();
        mosq = mosquitto_new(CLIENT_ID, true, nullptr);
        if (mosquitto_connect(mosq, SERVER_ADDRESS, SERVER_PORT, 60) != 0) {
            // Handle connection failure
            std::cerr << "Failed to connect to MQTT broker" << std::endl;
            std::cerr << "Start the Mosquitto/MQTT broker and restart the process" << std::endl;
            exit(1);
        }
        mosquitto_subscribe(mosq, nullptr, TOPIC_REQUEST_COMM, 0); // Subscribe to the added topic
        // Example of publishing an open communication channel request
        //publishOpenCommChannelResponse();
        //mosquitto_subscribe(mosq, nullptr, TOPIC_READ_PGN_REQUEST, 0);

        // Set up the callback for receiving messages
        mosquitto_message_callback_set(mosq, messageCallback);
    }

    ~Translator() {
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
    }

    void publishOpenCommChannelResponse() {
        json payload = {
            {"appID", "data_sampler"},
            {"sequenceNo", "2"},
            {"toolAddress", "0x18DAFB00"},
            {"ecuAddress", "0x18DA00FB"},
            {"canFormat", {
                {"canPhysReqFormat", "0x07"},
                {"canRespUSDTFormat", "0x07"}
            }},
            {"resourceName", "CANTP_UDS_on_CAN"}
        };

        std::cout << "Publishing Comm Response to the requestor" << std::endl;
        mosquitto_publish(mosq, nullptr, TOPIC_RESPONSE_COMM, payload.dump().size(), payload.dump().c_str(), 0, false);
    }

    void loop() {
        // Keep processing messages
        while (true) {
            mosquitto_loop(mosq, -1, 1);
        }
    }

private:
    struct mosquitto* mosq;

    static void messageCallback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
        if (message->topic == std::string(TOPIC_READ_PGN_REQUEST)) {
            std::cout << "Received READ PGNs request: " << static_cast<char*>(message->payload) << std::endl;

            // Handle the request
            json request = json::parse(static_cast<char*>(message->payload));
            std::string connectionID = request["connectionID"];
            std::string sequenceNo = request["sequenceNo"];
            auto pgnNo = request["pgnNo"];

            std::cout << "Received PGNs are: " << pgnNo << std::endl;

            // Create a response
            json response = {
                {"appID", "data_sampler"},
                {"connectionID", connectionID},
                {"sequenceNo", sequenceNo},
                {"data", json::array()},
                {"responseCode", "0"}
            };

            // Limit PGNs to supported ones
            for (const auto& pgn : pgnNo) {
                if (std::find(SUPPORTED_PGNS.begin(), SUPPORTED_PGNS.end(), pgn) != SUPPORTED_PGNS.end()) {
                    std::cout << "PGN " << pgn << " is supported" << std::endl;

                    json spnArray = json::array(); // Create a new JSON array for SPNs
                    // Here, you would populate spnArray with SPN values if needed.

                    // Create a new JSON object for the PGN data
                    json pgnData = {
                        {"decoded", true},
                        {"pgnNo", pgn},
                        {"rawData", std::string("SampleDataFor_") + pgn.get<std::string>()},
                        {"spn", spnArray}
                    };

                    // Push the PGN data object to the response
                    response["data"].push_back(pgnData);
                } else {
                    std::cout << "ALERT: PGN " << pgn << " is not supported" << std::endl;
                }
            }

            // Publish the response
            std::cout << "Publishing READPGNS response" << std::endl;
            mosquitto_publish(mosq, nullptr, TOPIC_READ_PGN_RESPONSE, response.dump().size(), response.dump().c_str(), 0, false);
        } else if (message->topic == std::string(TOPIC_REQUEST_COMM)) {
            std::cout << "Received Open Comm Channel request: " << static_cast<char*>(message->payload) << std::endl;

            // Parse the request JSON
            json request = json::parse(static_cast<char*>(message->payload));

            // Extract relevant information from the request (e.g., appID, sequenceNo, toolAddress, ecuAddress, etc.)
            std::string appID = request["appID"];
            std::string sequenceNo = request["sequenceNo"];
            // ... (extract other fields as needed)

            // Process the request (e.g., open a communication channel, initialize resources, etc.)

            // Send a response to the client
            json response = {
                {"appID", request["appID"]},
                {"connectionID", "your_connection_id"},
                {"sequenceNo", sequenceNo},
                {"responseCode", "0"} // Or other appropriate response code
            };

            std::cout << "Publishing Comm Response to the requestor" << std::endl;
            mosquitto_publish(mosq, nullptr, TOPIC_RESPONSE_COMM, response.dump().size(), response.dump().c_str(), 0, false);
        }
    }
};

int main() {
    Translator translator;

    // Start the message loop
    translator.loop();

    return 0;
}
