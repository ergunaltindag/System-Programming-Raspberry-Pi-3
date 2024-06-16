#include "SocketConLib.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <limits>

// Menü yazdırma fonksiyonu
void printMenu() {
    std::cout << "***** Menu *****\n";
    std::cout << "1. Sensör Durumu\n";
    std::cout << "2. Sürücü Durumu Değiştir\n";
    std::cout << "3. Otomatik Denetim\n";
    std::cout << "4. Environment Sensor Verileri\n";
    std::cout << "5. Bağlantıyı Kapat\n";
    std::cout << "6. Key\n";
    std::cout << "****************\n";
    std::cout << "Seçim: ";
}

// Mesaj gönderme ve yanıt alma fonksiyonu
bool sendMessage(SocketConnection& socket, const std::string& message, std::string& response) {
    if (!socket.send(message)) {
        std::cerr << "Error: Failed to send message to server\n";
        return false;
    }
    if (!socket.receive(response)) {
        std::cerr << "Error: Failed to receive response from server\n";
        return false;
    }
    return true;
}

int main() {
    SocketConnection clientSocket;

    // Sunucuya bağlan
    if (!clientSocket.initAsClient("10.42.0.1", 7001)) {  // IP adresini ve port numarasını uygun şekilde değiştirin
        std::cerr << "Error: Failed to connect to server\n";
        return 1;
    }

    std::string message;
    std::string response;
    bool isConnected = true;

    // switch-case dışında tanımlanacak değişkenler
    std::string lastResponse = "";
    auto startTime = std::chrono::steady_clock::now();

    while (isConnected) {
        printMenu();

        int choice;
	int denetim = 0;
        std::cin >> choice;

        // Geçersiz girişleri temizleyelim
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                message = "sensorDurum";
                if (sendMessage(clientSocket, message, response)) {
                    std::cout << "Sensör Durumu: " << response << std::endl;
                } else {
                    isConnected = false;
                }
                break;
            case 2:
                message = "relayDurum";
                if (sendMessage(clientSocket, message, response)) {
                    if (response == "0") {
                        std::cout << "Sürücü Durumu: ON\nOFF yapmak için '1' a basın: ";
                    } else if (response == "1") {
                        std::cout << "Sürücü Durumu: OFF\nON yapmak için '0' e basın: ";
                    }
                    std::cin >>denetim;
                    if (denetim == 0) {
                        message = "relayTrue";
                        sendMessage(clientSocket, message, response);
                    } 
                    else if (denetim == 1) {
                        message = "relayFalse";
                        sendMessage(clientSocket, message, response);
                    } 
                    else {
                        std::cout << "Hatalı Tuşlama\n";
                        continue;
                    }
                } else {
                    isConnected = false;
                }
                break;
            case 3: {
                int duration;
                std::cout << "Otomatik Denetim: Kaç saniye kalmak istersiniz? : ";
                std::cin >> duration;

                std::cout << "Otomatik Denetim: " << duration << " saniye sürecek.\n";
                auto start = std::chrono::steady_clock::now();
                message = "sensorTip";
                sendMessage(clientSocket, message, response);
                std::cout << response;
                while (true) {
                    // Mesaj gönder
                    message = "sensorDurum";
                    if (!sendMessage(clientSocket, message, response)) {
                        isConnected = false;
                        break;
                    }

                    // Yanıtı kontrol et
                    if (response != lastResponse) {
                        if (response == "1") {
                            message = "relayTrue";
                        } else if (response == "0") {
                            message = "relayFalse";
                        } else {
                            std::cerr << "Unexpected response: " << response << std::endl;
                            break;
                        }
                        if (!sendMessage(clientSocket, message, response)) {
                            isConnected = false;
                            break;
                        }
                        lastResponse = response;
                    }

                    // Zamanı kontrol et
                    auto now = std::chrono::steady_clock::now();
                    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
                    if (elapsedSeconds >= duration) {
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                std::cout  << "\n"<< duration << " saniye doldu, otomatik denetim sona eriyor.\n";
                break;
            }
            case 4:
                std::cout << "Sıcaklık için 1'e basınız.\n";
                std::cout << "Nem için 2'ye basınız :";
                std::cin >> message;
                if (message == "1") {
                    message = "temp";
                } else if (message == "2") {
                    message = "huminity";
                } else {
                    std::cout << "Hatalı Tuşlama\n";
                    continue;
                }
                if (sendMessage(clientSocket, message, response)) {
                    if (message == "temp") {
                        std::cout << "\nSıcaklık: " << response << std::endl;
                    } else {
                        std::cout << "\nNem: " << response << std::endl;
                    }
                } else {
                    isConnected = false;
                }
                break;
            case 5:
                message = "kapat";
                if (sendMessage(clientSocket, message, response)) {
                    isConnected = false;
                } else {
                    isConnected = false;
                }
                break;
            case 6:
                message = "key";
                if (sendMessage(clientSocket, message, response)) {
                    std::cout << "KEY: " << response << std::endl;
                } else {
                    isConnected = false;
                }
                break;
            default:
                std::cout << "Geçersiz seçim. Lütfen tekrar deneyin.\n";
                continue;
        }
    }

    // Bağlantıyı kapat
    clientSocket.release();

    return 0;
}



