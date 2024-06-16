#include <iostream>
#include "RelayLib.h"
#include <gpiod.hpp>
#include <string.h>
#include <unistd.h>
using namespace std;

RelayLib::RelayLib() {
    chipName = "gpiochip0";
    chip = nullptr;
    lineRelay = nullptr;
}

RelayLib::~RelayLib() {
  
    gpiod_line_release(lineRelay);
    gpiod_chip_close(chip);
}

void RelayLib::init() {
    chip = gpiod_chip_open_by_name(chipName.c_str());
    lineRelay = gpiod_chip_get_line(chip, 27);
    gpiod_line_request_output(lineRelay, "Relay", 1);
}


void RelayLib::release() {
   
    gpiod_line_release(lineRelay);
    gpiod_chip_close(chip);

}

void RelayLib::set(bool state) {
    if (!state) {
        cout << "Röle kapalı..." << endl;
        gpiod_line_set_value(lineRelay, 1);
    } else {
        cout << "Röle açık..." << endl;
        gpiod_line_set_value(lineRelay, 0);
    }
}
int RelayLib::read() {
   return gpiod_line_get_value(lineRelay);
}