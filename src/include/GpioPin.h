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
        GpioPin(unsigned char gpioNumber, PinDirection direction);
        GpioPin(unsigned char gpioNumber, PinDirection direction, bool initialValue);
        virtual ~GpioPin();

        bool GetValue();
        bool GetValue(bool invert);
        bool SetValue(bool value);
        bool SetDirection(PinDirection direction);
        GpioPin(const GpioPin&) = delete;               //Delete copy constructor
        GpioPin& operator=(const GpioPin&) = delete;    //Delete copy assignment

    private:
        bool Init(unsigned char gpioNumber, PinDirection direction, bool initialValue);
        bool ExportPin();
        bool UnExportPin();
        bool WriteToFile(std::string file, std::string value);

        std::unique_ptr<std::ofstream> valueStream;
        unsigned char gpioNumber;
};

#endif // GPIOPIN_H
