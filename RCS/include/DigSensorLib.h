#ifndef DIGSENSORLIB_H
#define DIGSENSORLIB_H

#include <string>

class DigSensorLib {
public:
    DigSensorLib();
    ~DigSensorLib();
    void init();
    void release();
    int read();

private:
    std::string chipName; 
    struct gpiod_chip *chip;
    struct gpiod_line *lineSensor;
};
#endif