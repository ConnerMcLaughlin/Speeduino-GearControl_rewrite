#ifndef SENSORS_H
#define SENSORS_H

#include "Arduino.h"

void initialiseADC();
void readAnalog(byte AinCH);
void readDigitalIn(byte DinCH);
void getAnalogBYoffset(byte AinCH, byte readloop);

uint16_t tempReading;
    
#endif
