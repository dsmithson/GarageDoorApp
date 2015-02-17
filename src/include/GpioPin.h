#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <string>
#include <memory>
#include <ostream>
#include "DataPins.h"

enum class PinDirection
{
    In,
    Out
};

class GpioPin
{
    public:
        GpioPin(std::string gpioNumber, PinDirection direction);
        virtual ~GpioPin();

        int GetValue();
        bool SetValue(bool value);
        GpioPin(const GpioPin&) = delete;               //Delete copy constructor
        GpioPin& operator=(const GpioPin&) = delete;    //Delete copy assignment

    private:
        bool ExportPin();
        bool UnExportPin();
        bool SetDirection(PinDirection direction);
        bool WriteToFile(std::string file, std::string value);

        std::unique_ptr<std::ofstream> valueStream;
        std::string gpioNumber;
};

#endif // GPIOPIN_H
