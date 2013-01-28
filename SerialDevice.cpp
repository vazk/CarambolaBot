#include "SerialDevice.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include <string.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <iostream>


SerialDevice::SerialDevice()
 : mDesc(INVALID_DESC)
{
}

ssize_t
SerialDevice::write(const uint8_t* data, size_t size)
{
    if(mDesc == INVALID_DESC) {
        return 0;
    }
    ssize_t numWrite = ::write(mDesc, data, size); 
    if(numWrite > 0 && (size_t)numWrite == size) {
        mLastActivityMs = milliseconds();
    } else {
        mDesc = INVALID_DESC;
    }
    return numWrite;
}

ssize_t 
SerialDevice::read(uint8_t* data, size_t size)
{
    if(mDesc == INVALID_DESC) {
        return 0;
    }
    ssize_t numRead = ::read(mDesc, data, size);
    if(numRead > 0 && (size_t)numRead == size) {
        mLastActivityMs = milliseconds();
    } else {
        mDesc = INVALID_DESC;
    }
    return numRead;
}

void 
SerialDevice::close()
{
    if(mDesc != INVALID_DESC) {
        ::close(mDesc);
    }
    mDesc = INVALID_DESC;
}

uint32_t 
SerialDevice::lastActivityMs() const 
{ 
    return mLastActivityMs; 
}

bool
SerialDevice::isOpen() const 
{
    return mDesc != INVALID_DESC;
}
