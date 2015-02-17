#include <iostream>
#include <string>
#include <signal.h>
#include "DataPins.h"
#include "GpioPin.h"
#include "ShiftRegister.h"
#include "ShiftRegisterPins.h"

using namespace std;

int main()
{
    cout << "Starting Up" << endl;
    DataPins pins;

    ShiftRegister shiftRegister(pins.D1, pins.D2, pins.D0);

    cout << "Cycling shift register pins:" << endl;
    for(int i=0 ; i<8 ; i++)
    {
        cout << endl << "Cycling pin " << i << " on...";
        shiftRegister.SetBit(i, true);
        sleep(1);

        cout << endl << "Cycling pin " << i << " off...";
        shiftRegister.SetBit(i, false);
        sleep(1);
    }

    cout << "Done" << endl;
    return 0;
}
