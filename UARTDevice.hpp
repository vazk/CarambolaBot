#ifndef UART_DEVICE_HPP
#define UART_DEVICE_HPP

#include "SerialDevice.hpp"
#include <sys/types.h>
#include <stdint.h>
#include <termios.h>
#include <string>


class UARTDevice : public SerialDevice
{
public:
    enum {INVALID_DESC = -1};
    struct UARTDeviceConfig
    {
        enum BitRate { 
            BAUD_0      =  B0,
            BAUD_50     =  B50,
            BAUD_75     =  B75,
            BAUD_110    =  B110,
            BAUD_134    =  B134,
            BAUD_150    =  B150,
            BAUD_200    =  B200,
            BAUD_300    =  B300,
            BAUD_600    =  B600,
            BAUD_1200   =  B1200,
            BAUD_1800   =  B1800,
            BAUD_2400   =  B2400,
            BAUD_4800   =  B4800,
            BAUD_9600   =  B9600,
            BAUD_19200  =  B19200,
            BAUD_38400  =  B38400,
            BAUD_57600  =  B57600,
            BAUD_115200 =  B115200
        };
        BitRate  baudRate;
        std::string deviceName;
    };

public:
    ~UARTDevice();
    bool    open(const UARTDeviceConfig& cfg);
    void    close(); 

private:
    struct termios mOrigSettings;
};

#endif // UART_DEVICE_HPP
