#include "GpioPin.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

GpioPin::GpioPin(unsigned char gpioNumber, PinDirection direction, bool initialValue)
    : gpioNumber(gpioNumber)
{
    Init(gpioNumber, direction, initialValue);
}

GpioPin::GpioPin(unsigned char gpioNumber, PinDirection direction)
    : gpioNumber(gpioNumber)
{
    Init(gpioNumber, direction, false);
}

GpioPin::~GpioPin()
{
    if(valueStream != nullptr)
        valueStream->close();

    UnExportPin();
}

bool GpioPin::Init(unsigned char gpioNumber, PinDirection direction, bool initialValue)
{
    valueStream = nullptr;

    if(ExportPin())
    {
        if(SetDirection(direction) && direction == PinDirection::Out)
        {
            return SetValue(initialValue);
        }
    }
    return false;
}

bool GpioPin::ExportPin()
{
    cout << "Exporting pin " << this->gpioNumber << endl;

    string exportPath = "/sys/class/gpio/export";
    return WriteToFile(exportPath, to_string(this->gpioNumber));
}

bool GpioPin::UnExportPin()
{
    cout << "UnExporting pin " << this->gpioNumber << endl;

    string exportPath = "/sys/class/gpio/unexport";
    return WriteToFile(exportPath, to_string(this->gpioNumber));
}

bool GpioPin::SetDirection(PinDirection direction)
{
    if(valueStream != nullptr)
    {
        valueStream->close();
        valueStream = nullptr;
    }

    string gpioPath = "/sys/class/gpio/gpio" + to_string(this->gpioNumber) + "/direction";
    string value = (direction == PinDirection::In ? "in" : "out");
    bool success = WriteToFile(gpioPath, value);

    if(valueStream != nullptr)
    {
        valueStream->close();
        valueStream = nullptr;
    }

    if(success && direction == PinDirection::Out)
    {
        string gpioPath = "/sys/class/gpio/gpio" + to_string(this->gpioNumber) + "/value";
        valueStream = unique_ptr<ofstream>(new ofstream(gpioPath.c_str()));
    }
    return success;
}

bool GpioPin::GetValue()
{
    return GetValue(false);
}

bool GpioPin::GetValue(bool invert)
{
    bool response = false;
    string gpioPath = "/sys/class/gpio/gpio" + to_string(this->gpioNumber) + "/value";
    ifstream stream(gpioPath.c_str());
    if(stream < 0)
    {
        cout << " OPERATION FAILED: Unable to get file stream to read GPIO value for " << to_string(this->gpioNumber) << "." << endl;
    }
    else
    {
        string value;
        stream >> value;
        if(value == "1")
            response = true;
    }

    if(invert)
        response = !response;

    return response;
}

bool GpioPin::SetValue(bool value)
{
    if(valueStream == nullptr)
        return false;

    *valueStream << (value ? "1" : "0");
    valueStream->flush();
    return true;
    //string gpioPath = "/sys/class/gpio/gpio" + this->gpioNumber + "/value";
    //return WriteToFile(gpioPath, value ? "1" : "0");
}

bool GpioPin::WriteToFile(string file, string value)
{
    ofstream fileStream(file.c_str());
    if(fileStream < 0)
    {
        cout << " OPERATION FAILED: Unable to Open FileStream '" << file << "' to write value '" << value << "'." << endl;
        return false;
    }

    fileStream << value;
    fileStream.close();
    return true;
}
