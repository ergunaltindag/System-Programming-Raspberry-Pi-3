#include <iostream>
#include "DigSensorLib.h"
#include <gpiod.hpp>
#include <string.h>
#include <unistd.h>
using namespace std;

DigSensorLib::DigSensorLib() {
    chipName = "gpiochip0";
    chip = nullptr;
    lineSensor = nullptr;
}

DigSensorLib::~DigSensorLib() {
  
    gpiod_line_release(lineSensor);
    gpiod_chip_close(chip);
}

void DigSensorLib::init() {
    chip = gpiod_chip_open_by_name(chipName.c_str());
    lineSensor = gpiod_chip_get_line(chip, 17);
    gpiod_line_request_input(lineSensor, "Sensor");
}


void DigSensorLib::release() {
   
    gpiod_line_release(lineSensor);
    gpiod_chip_close(chip);

}

int DigSensorLib::read() {
   return gpiod_line_get_value(lineSensor);

}
