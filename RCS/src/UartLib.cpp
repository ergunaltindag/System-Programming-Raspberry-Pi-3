#include "UartLib.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <errno.h>

UartLib::UartLib() : uart0_filestream(-1) {}

UartLib::~UartLib() {
    close();
}

int UartLib::open() {
    uart0_filestream = ::open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_filestream == -1) {
        std::cerr << "UART açılamıyor. Başka bir uygulama tarafından kullanılmadığından emin olun\n";
        return -1;
    }

    // Engellemesiz mod
    fcntl(uart0_filestream, F_SETFL, FNDELAY);

    // UART'ı yapılandırın
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return 0;
}

void UartLib::close() {
    if (uart0_filestream != -1) {
        ::close(uart0_filestream);
        uart0_filestream = -1;
    }
}

std::string UartLib::send(const std::string& data) {
    if (uart0_filestream == -1) {
        return "UART TX hatası";
    }

    unsigned char tx_buffer[20];
    memset(tx_buffer, 0, sizeof(tx_buffer));
    strncpy(reinterpret_cast<char*>(tx_buffer), data.c_str(), sizeof(tx_buffer) - 1);

    int count = write(uart0_filestream, tx_buffer, strlen(reinterpret_cast<char*>(tx_buffer)));
    if (count < 0) {
        std::cerr << "UART TX hatası\n";
        return "UART TX hatası";
    }

    std::cout << "Gönderilen veri: " << tx_buffer << std::endl;

    // Yanıtı beklemek için bir süre döngü ile bekleme
    std::string response;
    for (int i = 0; i < 5; ++i) { // 5 kez deneme yap
        response = receive();
        if (response != "UART RX hatası: Okunacak veri yok." && !response.empty()) {
            break;
        }
        usleep(100000); // 100 ms bekle
    }
    return response;
}

std::string UartLib::receive() {
    if (uart0_filestream == -1) {
        return "UART RX hatası: UART cihazı açılmamış.";
    }

    unsigned char rx_buffer[256];
    int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);
    if (rx_length < 0) {
        if (errno == EAGAIN) {
            return "UART RX hatası: Okunacak veri yok.";
        } else {
            perror("UART RX hatası");
            return "UART RX hatası: Veri okunamıyor.";
        }
    }
    if (rx_length == 0) {
        return "UART RX hatası: Veri yok.";
    }
    rx_buffer[rx_length] = '\0';
    return std::string(reinterpret_cast<char*>(rx_buffer));
}
