#include "SocketConLib.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Fonksiyon: Mesajı belirli bir node'a gönderir ve yanıt alır
bool sendMessageToNode(SocketConnection &nodeSocket, const std::string &message, std::string &response) {
    if (!nodeSocket.send(message)) {
        std::cout << "Error: Failed to send message to Node\n";
        return false;
    }

    std::cout << "Message sent to Node\n";

    if (!nodeSocket.receive(response)) {
        std::cerr << "Error: Failed to receive response from Node\n";
        return false;
    }

    std::cout << "Received response from Node: " << response << std::endl;
    return true;
}

int main() {
    SocketConnection serverSocket;
    SocketConnection clientSocketDigitalIO;
    SocketConnection clientSocketEnvSensor;

    // Sunucuyu başlat
    if (!serverSocket.initAsServer(7001)) {
        std::cout << "Error: Failed to start server on port 7001\n";
        return 1;
    }

    // DigitalIO ve EnvSensor için istemci bağlantılarını kur
    if (!clientSocketDigitalIO.initAsClient("127.0.0.1", 7002)) {
        std::cout << "Error: Failed to connect to DigitalIO node on port 7002\n";
        return 1;
    }
    if (!clientSocketEnvSensor.initAsClient("127.0.0.1", 7003)) {
        std::cout << "Error: Failed to connect to EnvSensor node on port 7003\n";
        return 1;
    }

    bool isConnected = true;

    while (isConnected) {
        std::string message;
        if (!serverSocket.receive(message)) {
            std::cout << "Error: Failed to receive message. Resetting connection...\n";
            serverSocket.release(); // Bağlantıyı serbest bırak ve yeniden başlat
            isConnected = false;
            continue;
        }

        std::cout << "Received message: " << message << std::endl;

        std::string response;
        bool messageSent = false;

        if (message == "temp" || message == "huminity") {
            messageSent = sendMessageToNode(clientSocketEnvSensor, message, response);
        } else if (message == "relayDurum" || message == "relayFalse" || message == "relayTrue" || 
                   message == "key" || message == "sensorDurum" || message == "sensorTip") {
            messageSent = sendMessageToNode(clientSocketDigitalIO, message, response);
        } else if (message == "kapat") {
            bool sentToDigitalIO = sendMessageToNode(clientSocketDigitalIO, message, response);
            bool sentToEnvSensor = sendMessageToNode(clientSocketEnvSensor, message, response);
            if (sentToDigitalIO && sentToEnvSensor) {
                messageSent = true;
            }
        } else {
            std::cout << "Error: Unknown message received\n";
        }

        if (messageSent) {
            if (!serverSocket.send(response)) {
                std::cout << "Error: Failed to send response to client\n";
            }
        }

        if (message == "kapat") {
            std::cout << "Closing connection...\n";
            serverSocket.release();
            isConnected = false;
        }
    }

    std::cout << "Server shutting down...\n";
    return 0;
}
