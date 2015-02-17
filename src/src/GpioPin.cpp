#include "GpioPin.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

GpioPin::GpioPin(string gpioNumber, PinDirection direction)
    : gpioNumber(gpioNumber)
{
    valueStream = nullptr;

    if(ExportPin())
    {
        if(SetDirection(direction))
        {
            string gpioPath = "/sys/class/gpio/gpio" + this->gpioNumber + "/value";
            valueStream = unique_ptr<ofstream>(new ofstream(gpioPath.c_str()));
        }
    }
}

GpioPin::~GpioPin()
{
    UnExportPin();

    if(valueStream != nullptr)
        valueStream->close();
}

bool GpioPin::ExportPin()
{
    cout << "Exporting pin " << this->gpioNumber << endl;

    string exportPath = "/sys/class/gpio/export";
    return WriteToFile(exportPath, this->gpioNumber);
}

bool GpioPin::UnExportPin()
{
    cout << "UnExporting pin " << this->gpioNumber << endl;

    string exportPath = "/sys/class/gpio/unexport";
    return WriteToFile(exportPath, this->gpioNumber);
}

bool GpioPin::SetDirection(PinDirection direction)
{
    string gpioPath = "/sys/class/gpio/gpio" + this->gpioNumber + "/direction";

    string value = (direction == PinDirection::In ? "in" : "out");
    return WriteToFile(gpioPath, value);
}

int GpioPin::GetValue()
{
    string gpioPath = "/sys/class/gpio/gpio" + this->gpioNumber + "/value";
    ifstream stream(gpioPath.c_str());
    if(stream < 0)
    {
        cout << " OPERATION FAILED: Unable to get file stream to read GPIO value for " << this->gpioNumber << "." << endl;
        return -1;
    }

    string value;
    stream >> value;
    stream.close();

    return stoi(value);
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
