#include <iostream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "UartLib.h"

#define PORT 7003
#define BUFFER_SIZE 1024

void *EnvSensorNode(void *parameter) {
    UartLib uart;
    if (uart.open() == -1) {
        pthread_exit((void *)-1);
    }

    // Soket oluşturma
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Hata: Soket oluşturulamadı." << std::endl;
        uart.close();
        pthread_exit((void *)-1);
    }

    // Adres bilgileri
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // Soket ve adresi bağlama
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Hata: Soket bağlanamadı." << std::endl;
        close(serverSocket);
        uart.close();
        pthread_exit((void *)-1);
    }

    // Bağlantıları dinleme
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Hata: Dinleme hatası." << std::endl;
        close(serverSocket);
        uart.close();
        pthread_exit((void *)-1);
    }

    std::cout << "Sunucu dinleniyor..." << std::endl;
    bool runServer = true;

    while (runServer) {
        // Bağlantı kabul etme
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cerr << "Hata: Bağlantı kabul edilemedi." << std::endl;
            continue;
        }

        std::cout << "Bağlantı kabul edildi." << std::endl;
        while (runServer) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            // İstekleri okuma
            ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead <= 0) {
                std::cerr << "Hata: İstek okunamadı." << std::endl;
                close(clientSocket);
                break;
            }

            std::cout << "İstek alındı: " << buffer << std::endl;

            // İstek tipine göre yanıt oluşturma
            std::string uartResponse;
            if (strncmp(buffer, "temp", 4) == 0) {
                uartResponse = uart.send("t");
            } else if (strncmp(buffer, "huminity", 8) == 0) {
                uartResponse = uart.send("h");
            } else if (strncmp(buffer, "kapat", 5) == 0) {
                uartResponse = "kapat ok";
                runServer = false; // Sunucuyu kapat
            } else {
                uartResponse = "Bilinmeyen istek";
            }

            // Yanıtı gönderme
            send(clientSocket, uartResponse.c_str(), uartResponse.length(), 0);
        }
        // Bağlantıyı kapatma
        close(clientSocket);
    }

    // Soket kapatma
    close(serverSocket);
    uart.close();
    std::cout << "Sunucu kapatıldı." << std::endl;

    pthread_exit(NULL);
}

void *VirtualEnvSensorNode(void *parameter) {
    UartLib uart;
    if (uart.open() == -1) {
        pthread_exit((void *)-1);
    }

    while (true) {
        std::string received = uart.receive();
        if (received.empty() || received.find("UART RX hatası") != std::string::npos) {
            usleep(100000); // UART'tan veri gelmezse kısa bir süre bekleyin
            continue;
        }

        std::cout << "VirtualEnvSensorNode - Alınan veri: " << received << std::endl;

        std::string response;
        if (received == "t") {
            response = "temp 25.3"; // Sabit sıcaklık değeri
        } else if (received == "h") {
            response = "huminity 60.0"; // Sabit nem değeri
        } else {
            response = "Bilinmeyen komut";
        }

        std::string sendStatus = uart.send(response);
        if (sendStatus.find("UART TX hatası") != std::string::npos) {
            std::cerr << "UART üzerinden veri gönderilirken hata: " << response << std::endl;
        }
    }

    uart.close();
    pthread_exit(NULL);
}

int main() {
    pthread_t envThread, virtualEnvThread;

    // EnvSensorNode thread'i oluşturma
    if (pthread_create(&envThread, NULL, EnvSensorNode, NULL) != 0) {
        std::cerr << "EnvSensorNode thread'i oluşturulamadı." << std::endl;
        return -1;
    }

    // VirtualEnvSensorNode thread'i oluşturma
    if (pthread_create(&virtualEnvThread, NULL, VirtualEnvSensorNode, NULL) != 0) {
        std::cerr << "VirtualEnvSensorNode thread'i oluşturulamadı." << std::endl;
        return -1;
    }

    // Thread'lerin tamamlanmasını bekleme
    pthread_join(envThread, NULL);
    pthread_join(virtualEnvThread, NULL);

    return 0;
}
