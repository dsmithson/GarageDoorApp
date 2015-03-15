#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include "GpioPin.h"
#include <string>

class ShiftRegister
{
    public:
        ShiftRegister(unsigned char latchGpioNumber, unsigned char clockGpioNumber, unsigned char dataGpioNumber);
        virtual ~ShiftRegister();
        ShiftRegister(const ShiftRegister&) = delete;               //Delete copy constructor
        ShiftRegister& operator=(const ShiftRegister&) = delete;    //Delete copy assignment
        bool SetBit(const int bitIndex, bool value);

    private:
        bool Write();

        unsigned char data;
        GpioPin latchPin;
        GpioPin clockPin;
        GpioPin dataPin;
};

#endif // SHIFTREGISTER_H
