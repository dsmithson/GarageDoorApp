#include "ShiftRegister.h"

ShiftRegister::ShiftRegister(unsigned char latchGpioNumber, unsigned char clockGpioNumber, unsigned char dataGpioNumber)
    : data(0), latchPin(latchGpioNumber, PinDirection::Out), clockPin(clockGpioNumber, PinDirection::Out), dataPin(dataGpioNumber, PinDirection::Out)
{
    //Clear the shift register
    Write();
}

ShiftRegister::~ShiftRegister()
{
    //dtor
}

bool ShiftRegister::SetBit(const int bitIndex, bool value)
{
    if(value)
        data |= (unsigned char)(0x01 << bitIndex);
    else
        data &= ~(unsigned char)(0x01 << bitIndex);

    return Write();
}

bool ShiftRegister::Write()
{
    //Turn off the output so the pins don't light up while shifting bits
    latchPin.SetValue(false);

    //Shift our data bits (Most significant bit first), and toggle the clock as we go
    for(int i=0 ; i<8 ; i++)
    {
        bool isOn = (data & (0x80 >> i)) != 0;
        dataPin.SetValue(isOn);
        clockPin.SetValue(true);
        //usleep(100);
        clockPin.SetValue(false);
        //usleep(100);
    }

    //Turn on the output
    latchPin.SetValue(true);

    return true;
}
