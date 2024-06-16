#include "KeypadLib.h"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
KeypadLib::KeypadLib()
{
    chipName = "gpiochip0";
}
KeypadLib::~KeypadLib() {
    // Release lines and chip
    for(int i=0; i<4; i++)
        gpiod_line_release(lineKeyCol[i]);
    for(int i=0; i<4; i++)
        gpiod_line_release(lineKeyRow[i]);
    gpiod_chip_close(chip);
}

void KeypadLib::init()
{
    chip=gpiod_chip_open_by_name(chipName.c_str());
 // Open GPIO lines
    for(int i=0;i<4;i++)
        lineKeyCol[i]=gpiod_chip_get_line(chip,keyGPIOCol[i]);
    for(int i=0;i<4;i++)
        lineKeyRow[i]=gpiod_chip_get_line(chip,keyGPIORow[i]);
    // Open lines for output and input
    for(int i=0;i<4;i++)
        gpiod_line_request_output(lineKeyCol[i],"COL",0);
    for(int i=0;i<4;i++)
        gpiod_line_request_input(lineKeyRow[i],"ROW");
}
char KeypadLib::getKey()
{
    int i,j;
    for(i=0;i<4;i++){
        //Set high first column
        gpiod_line_set_value(lineKeyCol[i],1);
        for(j=0;j<4;j++){
            if(gpiod_line_get_value(lineKeyRow[j])==1)
                return key[j][i];
        }
        gpiod_line_set_value(lineKeyCol[i],0);
    }
    return 'n';
}
void KeypadLib::release()
{
    // Release lines and chip
    for(int i=0;i<4;i++)
        gpiod_line_release(lineKeyCol[i]);
    for(int i=0;i<4;i++)
        gpiod_line_release(lineKeyRow[i]);
    gpiod_chip_close(chip);
}
