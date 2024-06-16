#ifndef UARTLIB_H
#define UARTLIB_H

#include <string>
#include <termios.h>

class UartLib {
public:
    UartLib();
    ~UartLib();

    int open();
    void close();
    std::string send(const std::string& data);
    std::string receive();

private:
    int uart0_filestream;
    struct termios options;
};

#endif // UARTLIB_H
