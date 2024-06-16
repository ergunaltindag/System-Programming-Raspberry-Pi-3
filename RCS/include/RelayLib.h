#ifndef RELAYLIB_H
#define RELAYLIB_H

#include <string>
using namespace std;
class RelayLib {
public:
    RelayLib();
    ~RelayLib();
    void init();
    void release();
    void set(bool state);
    int read();
private:
    std::string chipName; 
    struct gpiod_chip *chip;
    struct gpiod_line *lineRelay;
};

#endif