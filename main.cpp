#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <mqtt/async_client.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// MQTT parameters
const std::string SERVER_ADDRESS = "tcp://localhost:1883";
const std::string CLIENT_ID = "MQTTTranslatorClient";
const std::string CLIENT_ID_PUBLISHER = "MQTTPublisherClient";
const std::string TOPIC_PUBLISH = "diagstack/request/opencommchannel";
const std::string TOPIC_SUBSCRIBE = "diagstack/response/opencommchannel";
const std::string TOPIC_READ_PGN_REQUEST = "diagstack/request/readpgns";
const std::string TOPIC_READ_PGN_RESPONSE = "diagstack/response/readpgns";

// Supported PGNs
const std::vector<std::string> SUPPORTED_PGNS = {
    "F001", "FD9F", "FDA3", "FE9A", "FEA4", "FEEE", "FEF0", "FEF5", "FEF6", "FEF7"
};

class Translator {
public:
    Translator() : client(SERVER_ADDRESS, CLIENT_ID) {
        client.set_callback(*this);
        connect();
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

        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_PUBLISH, payload.dump());
        client.publish(pubmsg);
    }

    void subscribe() {
        client.subscribe(TOPIC_PUBLISH, 1);
        client.subscribe(TOPIC_READ_PGN_REQUEST, 1);
    }

    void on_message(const mqtt::const_message_ptr& msg) override {
        if (msg->get_topic() == TOPIC_PUBLISH) {
            handleOpenCommChannel(msg->to_string());
        } else if (msg->get_topic() == TOPIC_READ_PGN_REQUEST) {
            handleReadPGNs(msg->to_string());
        }
    }

private:
    mqtt::async_client client;

    void connect() {
        try {
            client.connect()->wait();
            std::cout << "Connected to MQTT broker!" << std::endl;
            subscribe();
        } catch (const mqtt::exception& exc) {
            std::cerr << "Error connecting: " << exc.what() << std::endl;
        }
    }

    void handleOpenCommChannel(const std::string& payload) {
        // Handle the open communication channel request
        std::cout << "Received OPEN COMM CHANNEL request: " << payload << std::endl;

        // Respond to the request
        json response = {
            {"appID", "data_sampler"},
            {"connectionID", "0x95400f60"},
            {"sequenceNo", "1"},
            {"responseCode", "0"}
        };

        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_SUBSCRIBE, response.dump());
        client.publish(pubmsg);
    }

    void handleReadPGNs(const std::string& payload) {
        // Handle read PGNs request
        json request = json::parse(payload);
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
                response["data"].push_back({
                    {"decoded", true},
                    {"pgnNo", pgn},
                    {"rawData", "SampleDataFor_" + pgn},
                    {"spn", json::array()}
                });
            }
        }

        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_READ_PGN_RESPONSE, response.dump());
        client.publish(pubmsg);
    }
};

int main() {
    Translator translator;

    // Publishing an open communication channel request
    translator.publishOpenCommChannel();

    // Keep the application running to listen for messages
    while (true) {
        translator.client.yield();
    }

    return 0;
}
