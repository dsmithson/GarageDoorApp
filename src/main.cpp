#include <iostream>
#include <string>
#include <signal.h>
#include "DataPins.h"
#include "GpioPin.h"
#include "ShiftRegister.h"
#include "ShiftRegisterPins.h"
#include <chrono>
#include <thread>
#include <signal.h>
#include <iostream>
#include <sstream>

using namespace std;

const unsigned char doorRelayOutputPin = 17;
const unsigned char lightRelayOutputPin = 18;
const unsigned char doorPositionInputPin = 27;
const unsigned char doorSwitchInputPin = 22;
const unsigned char lightSwitchInputPin = 23;
const unsigned char redLedOutputPin = 24;
const unsigned char blueLedOutputPin = 25;

const int puttonPressIterations = 400;
const int blinkIterations = 800;

int blinkCountdown = 0;
int doorRelayCountdown = 0;
int lightRelayCountdown = 0;
bool exitRequested = false;
bool doorButtonPressed = false;
bool lightButtonPressed = false;
bool doorClosed = false;
bool blinkOn = false;

bool hasValueChanged(GpioPin* inputPin, bool* lastValue)
{
    bool value = inputPin->GetValue(true);
    if(value != *lastValue)
    {
        *lastValue = value;
        return true;
    }
    return false;
}

void DecrementCountdown(GpioPin* relay, int* iterationsRemaining)
{
    if(*iterationsRemaining > 0)
    {
        if(--*iterationsRemaining == 0)
        {
            relay->SetValue(false);
        }
    }
}

void sig_handler(int sig)
{
    //Using write below instead of cout because this function must be reentrant
    write(0, "nCtrl^C pressed in sig handler", 32);
    exitRequested = true;
}

int main()
{
    //Hook to be signalled when the app gets a termination signal
    struct sigaction sigStruct;
    sigStruct.sa_handler = sig_handler;
    sigStruct.sa_flags = 0;
    sigemptyset(&sigStruct.sa_mask);
    if(sigaction(SIGINT, &sigStruct, NULL) == -1)
    {
        cout << "Failed to register sigaction" << endl;
        return -1;
    }

    //Setup our input switches
    GpioPin doorClosedSwitch(doorPositionInputPin, PinDirection::In);
    GpioPin doorButton(doorSwitchInputPin, PinDirection::In);
    GpioPin lightButton(lightSwitchInputPin, PinDirection::In);

    //Setup our LEDs and Relays
    GpioPin redLed(redLedOutputPin, PinDirection::Out);
    GpioPin blueLed(blueLedOutputPin, PinDirection::Out);
    GpioPin doorRelay(doorRelayOutputPin, PinDirection::Out);
    GpioPin lightRelay(lightRelayOutputPin, PinDirection::Out);

    //Set initial state
    doorClosed = doorClosedSwitch.GetValue(true);
    redLed.SetValue(!doorClosed);

    cout << "Environment initialized.  Starting main loop..." << endl;
    while(!exitRequested)
    {
        //Decrement pending iterations
        DecrementCountdown(&doorRelay, &doorRelayCountdown);
        DecrementCountdown(&lightRelay, &lightRelayCountdown);

        if(--blinkCountdown <= 0)
        {
            blinkOn = !blinkOn;
            blinkCountdown = blinkIterations;
            blueLed.SetValue(blinkOn);
        }

        //Check for door switch press event
        if(hasValueChanged(&doorButton, &doorButtonPressed) && doorButtonPressed && doorRelayCountdown <= 0)
        {
            //Need to activate the door relay
            doorRelayCountdown = puttonPressIterations;
            doorRelay.SetValue(true);
        }

        //Check for light switch press event
        if(hasValueChanged(&lightButton, &lightButtonPressed) && lightButtonPressed && lightRelayCountdown <= 0)
        {
            //Need to activate the door relay
            lightRelayCountdown = puttonPressIterations;
            lightRelay.SetValue(true);
        }

        //Update door position switch status
        if(hasValueChanged(&doorClosedSwitch, &doorClosed))
        {
            redLed.SetValue(!doorClosed);
        }

        //Sleep until next loop iteration
        std::chrono::milliseconds(25);
    }

    cout << "Main loop exited normally.  Exiting now..." << endl;
    blueLed.SetValue(false);
    return 0;
}

