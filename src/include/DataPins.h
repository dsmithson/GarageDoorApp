#ifndef DATAPINS_H
#define DATAPINS_H

#include <string>

class DataPins
{
    public:
        std::string D0;
        std::string D1;
        std::string D2;
        std::string D3;
        std::string D4;
        std::string D5;
        std::string D6;
        std::string D7;

        DataPins()
            : D0("17"), D1("18"), D2("27"), D3("22"), D4("23"), D5("24"), D6("25"), D7("4")
        {
        }

    private:

};

#endif // DATAPINS_H
