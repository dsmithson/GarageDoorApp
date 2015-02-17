#ifndef SHIFTREGISTERPINS_H
#define SHIFTREGISTERPINS_H


class ShiftRegisterPins
{
    public:
        const static int GarageDoorRelay =  0;
        const static int GarageLightRelay =  1;
        const static int GreenLED = 2;
        const static int RedLED = 3;
        const static int YellowLED = 4;

    private:
        ShiftRegisterPins() { }
};

#endif // SHIFTREGISTERPINS_H
