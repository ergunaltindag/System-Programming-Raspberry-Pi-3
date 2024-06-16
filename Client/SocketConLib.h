#ifndef SOCKETCONLIB_H
#define SOCKETCONLIB_H

#include <string>

class SocketConnection {
public:
    SocketConnection();
    ~SocketConnection();

    bool initAsServer(int port); // Server olarak başlatır
    bool initAsClient(const std::string& serverIP, int port); // Client olarak başlatır
    void release(); // Bağlantıyı sonlandırır
    bool send(const std::string& message); // Mesaj gönderir
    bool receive(std::string& message); // Mesaj alır
    bool acceptConnection();
private:
    int socketDescriptor;

    bool createSocket();
    bool bindSocket(int port);
    bool listenForConnection();
    bool connectToServer(const std::string& serverIP, int port);
    bool closeSocket();
};

#endif // SOCKETCONLIB_H
