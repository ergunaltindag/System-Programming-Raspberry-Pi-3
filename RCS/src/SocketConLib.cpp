#include "SocketConLib.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

SocketConnection::SocketConnection() {
    socketDescriptor = -1;
}

SocketConnection::~SocketConnection() {
    release();
}

bool SocketConnection::initAsServer(int port) {
    if (!createSocket()) return false;
    if (!bindSocket(port)) return false;
    if (!listenForConnection()) return false;
    if (!acceptConnection()) return false;
    return true;
}

bool SocketConnection::initAsClient(const std::string& serverIP, int port) {
    if (!createSocket()) return false;
    if (!connectToServer(serverIP, port)) return false;
    return true;
}

void SocketConnection::release() {
    closeSocket();
}

bool SocketConnection::send(const std::string& message) {
    int status = ::send(socketDescriptor, message.c_str(), message.size(), 0);
    if (status == -1) {
        std::cerr << "Error: Failed to send message\n";
        return false;
    }
    return true;
}

bool SocketConnection::receive(std::string& message) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(socketDescriptor, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == -1) {
        std::cerr << "Error: Failed to receive message\n";
        return false;
    }
    buffer[bytesReceived] = '\0';
    message = buffer;
    return true;
}

bool SocketConnection::createSocket() {
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return false;
    }
    return true;
}

bool SocketConnection::bindSocket(int port) {
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;
    if (bind(socketDescriptor, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Error: Failed to bind socket\n";
        return false;
    }
    return true;
}

bool SocketConnection::listenForConnection() {
    if (listen(socketDescriptor, SOMAXCONN) == -1) {
        std::cerr << "Error: Failed to listen for connection\n";
        return false;
    }
    return true;
}

bool SocketConnection::acceptConnection() {
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(socketDescriptor, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Error: Failed to accept connection\n";
        return false;
    }
    closeSocket();
    socketDescriptor = clientSocket;
    return true;
}

bool SocketConnection::connectToServer(const std::string& serverIP, int port) {
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &server.sin_addr);
    if (connect(socketDescriptor, (sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "Error: Failed to connect to server\n";
        return false;
    }
    return true;
}

bool SocketConnection::closeSocket() {
    if (socketDescriptor != -1) {
        close(socketDescriptor);
        socketDescriptor = -1;
    }
    return true;
}
