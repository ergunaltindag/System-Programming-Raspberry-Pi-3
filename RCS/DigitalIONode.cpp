#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include "RelayLib.h"
#include "DigSensorLib.h"
#include "KeypadLib.h"

#define PORT 7002
#define BUFFER_SIZE 1024

int main() {
    RelayLib relay;
    DigSensorLib sensor;
    KeypadLib keypad;

    // Soket oluşturma
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Hata: Soket oluşturulamadı." << std::endl;
        return -1;
    }

    // Adres bilgileri
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // Soket ve adresi bağlama
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Hata: Soket bağlanamadı." << std::endl;
        return -1;
    }

    // Bağlantıları dinleme
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Hata: Dinleme hatası." << std::endl;
        return -1;
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
        relay.init();
        while (runServer) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            // İstekleri okuma
            ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead == -1) {
                std::cerr << "Hata: İstek okunamadı." << std::endl;
                close(clientSocket);
                continue;
            }

            std::cout << "İstek alındı: " << buffer << std::endl;

            // İstek tipine göre yanıt oluşturma
            const char *response;
            if (strncmp(buffer, "relayDurum", 10) == 0) {
                int relayStatus = relay.read();
                response = std::to_string(relayStatus).c_str();
            } else if (strncmp(buffer, "relayTrue", 9) == 0) {
                relay.set(true);
                response = "relay true";
            } else if (strncmp(buffer, "relayFalse", 10) == 0) {
                relay.set(false);
                response = "relay false";
            } else if (strncmp(buffer, "kapat", 5) == 0) {
                response = "kapat ok";
                relay.release();
                runServer = false; // Sunucuyu kapat
            } else if (strncmp(buffer, "key", 3) == 0) {
                keypad.init();
                std::string input = "";
                char keypressed = 'n';
                char prevKeypressed = keypressed;
                while (keypressed != '#') {
                    prevKeypressed = keypressed;
                    keypressed = keypad.getKey();
                    switch(keypressed) {
                        case 'n':
                            break;
                        default:
                            if(prevKeypressed == 'n') {
                                input += keypressed;
                            }
                    }
                }
                keypad.release();
                response = input.c_str();
            } else if (strncmp(buffer, "sensorDurum", 11) == 0) {
                sensor.init();
                int sensorStatus = sensor.read();
                sensor.release();
                response = std::to_string(sensorStatus).c_str();
            } else if (strncmp(buffer, "sensorTip", 9) == 0) {
                response = "Sensor Tipi: Titreşim Sensörü";
            } else {
                response = "Geçersiz istek.";
            }

            // Yanıtı gönderme
            send(clientSocket, response, strlen(response), 0);
            }
        // Bağlantıyı kapatma
        close(clientSocket);
    }

    // Soket kapatma
    close(serverSocket);

    std::cout << "Sunucu kapatıldı." << std::endl;

    return 0;
}
