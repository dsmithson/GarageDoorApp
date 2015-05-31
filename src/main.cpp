//#include "pi_dht_read.h"
#include <iostream>
#include <string>
#include <signal.h>
#include "NetworkServer.h"
#include "DataPins.h"
#include "GpioPin.h"
#include "ShiftRegister.h"
#include <boost/thread.hpp>
#include "ShiftRegisterPins.h"
#include "dht11.h"
#include <chrono>
#include <thread>
#include <signal.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace boost::asio;

const unsigned char doorRelayOutputPin = 17;
const unsigned char lightRelayOutputPin = 18;
const unsigned char doorPositionInputPin = 27;
const unsigned char doorSwitchInputPin = 22;
const unsigned char lightSwitchInputPin = 23;
const unsigned char redLedOutputPin = 24;
const unsigned char blueLedOutputPin = 25;
const unsigned char tempHumidityPin = 4;

const int puttonPressIterations = 5;
const int blinkIterations = 10;
const int tempHumidityIterations = 600; // Roughly every 60 seconds

int blinkCountdown = 0;
int doorRelayCountdown = 0;
int lightRelayCountdown = 0;
int tempHumidityCountdown = 0;
bool exitRequested = false;
bool doorButtonPressed = false;
bool lightButtonPressed = false;
bool doorClosed = false;
bool blinkOn = false;
float temperature = 0;
float humidity = 0;

bool toggleDoorRequested = false;
bool toggleLightRequested = false;

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

void ReadTemperatureAndHumidity(dht11& sensor)
{

    int result = sensor.read();
    if(result == 0)
    {
        //TODO:  Log this or something
        temperature = sensor.temperature;
        humidity = sensor.humidity;
        cout << "Temperature: " << sensor.temperature << ", Humidity: " << sensor.humidity << endl;
    }
    else
    {
        cout << "Failed to read temp/humidity.  Received result code: " << result << endl;
    }
}

void sig_handler(int sig)
{
    //Using write below instead of cout because this function must be reentrant
    write(0, "Ctrl+C pressed in sig handler\r\n"", 32);
    exitRequested = true;
}

int main(int argc, char* args[])
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
    dht11 tempHumiditySensor(tempHumidityPin);

    //Set initial state
    doorClosed = doorClosedSwitch.GetValue(true);
    redLed.SetValue(!doorClosed);

    //Startup our network server
    io_service service;
    NetworkServer networkSrv(service, "password1",
        []() { return !doorClosed; },
        []() { return toggleDoorRequested = true; },
        []() { return toggleLightRequested = true; },
        []() { return temperature; },
        []() { return humidity; });

    boost::shared_ptr<boost::thread> serviceThread(new boost::thread(
        boost::bind(&boost::asio::io_service::run, &service)));

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

        if(--tempHumidityCountdown <= 0)
        {
            ReadTemperatureAndHumidity(tempHumiditySensor);
            tempHumidityCountdown = tempHumidityIterations;
        }

        //Check for door switch press event
        if(hasValueChanged(&doorButton, &doorButtonPressed) && doorButtonPressed && doorRelayCountdown <= 0)
        {
            toggleDoorRequested = true;
        }

        //Check for light switch press event
        if(hasValueChanged(&lightButton, &lightButtonPressed) && lightButtonPressed && lightRelayCountdown <= 0)
        {
            toggleLightRequested = true;
        }

        //Update door position switch status
        if(hasValueChanged(&doorClosedSwitch, &doorClosed))
        {
            redLed.SetValue(!doorClosed);
        }

        //////////////////////////////////////////////////////////////////////
        //Do we have a requested door/light toggle to process?
        if(toggleDoorRequested)
        {
            if(doorRelayCountdown == 0)
            {
                //Need to activate the door relay
                doorRelayCountdown = puttonPressIterations;
                doorRelay.SetValue(true);
            }
            toggleDoorRequested = false;
        }

        if(toggleLightRequested)
        {
            if(lightRelayCountdown == 0)
            {
                //Need to activate the door relay
                lightRelayCountdown = puttonPressIterations;
                lightRelay.SetValue(true);
            }
            toggleLightRequested = false;
        }

        //Sleep until next loop iteration
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    cout << "Main loop exited normally.  Exiting now..." << endl;
    blueLed.SetValue(false);
    return 0;
}

