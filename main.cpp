#include <iostream>
#include <string>
#include <vector>
#include <mosquitto.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// MQTT parameters
const char* SERVER_ADDRESS = "localhost";
const int SERVER_PORT = 1883;
const char* CLIENT_ID = "MQTTTranslatorClient";
const char* TOPIC_PUBLISH = "diagstack/request/opencommchannel";
const char* TOPIC_SUBSCRIBE = "diagstack/response/opencommchannel";
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
        mosquitto_connect(mosq, SERVER_ADDRESS, SERVER_PORT, 60);
        mosquitto_subscribe(mosq, nullptr, TOPIC_READ_PGN_REQUEST, 0);
        mosquitto_subscribe(mosq, nullptr, TOPIC_SUBSCRIBE, 0); // Subscribe to the added topic

        // Set up the callback for receiving messages
        mosquitto_message_callback_set(mosq, messageCallback);
    }

    ~Translator() {
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
    }

    void publishOpenCommChannel() {
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

        mosquitto_publish(mosq, nullptr, TOPIC_PUBLISH, payload.dump().size(), payload.dump().c_str(), 0, false);
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
                    json spnArray = json::array(); // Create a new JSON array for SPNs
                    // Here, you would populate spnArray with SPN values if needed.

                    // Create a new JSON object for the PGN data
                    json pgnData = {
                        {"decoded", true},
                        {"pgnNo", pgn},
                        {"rawData", std::string("SampleDataFor_") + pgn.get<std::string>()},
                        {"spn", spnArray}
                    };

                    response["data"].push_back(pgnData); // Push the PGN data object to the response
                }
            }

            mosquitto_publish(mosq, nullptr, TOPIC_READ_PGN_RESPONSE, response.dump().size(), response.dump().c_str(), 0, false);
        }
    }
};

int main() {
    Translator translator;

    // Example of publishing an open communication channel request
    translator.publishOpenCommChannel();

    // Start the message loop
    translator.loop();

    return 0;
}
