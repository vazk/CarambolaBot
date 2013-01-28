#ifndef SERIAL_DEVICE_HPP
#define SERIAL_DEVICE_HPP

#include <sys/types.h>
#include <stdint.h>
#include <termios.h>
#include <string>


class SerialDevice
{
public:
    enum { INVALID_DESC = -1 };

public:
    SerialDevice();
    ssize_t  write(const uint8_t* data, size_t size);
    ssize_t  read(uint8_t* data, size_t size);
    void     close(); 
    uint32_t lastActivityMs() const;
    bool     isOpen() const;

protected:
    int      mDesc;
    uint32_t mLastActivityMs;
};

#endif // SERIAL_DEVICE_HPP
