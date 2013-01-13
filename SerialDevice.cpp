#include "SerialDevice.hpp"
#include "Logger.hpp"
#include <string.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <iostream>


SerialDevice::SerialDevice()
 : mDesc(INVALID_DESC)
{
}

SerialDevice::~SerialDevice()
{
    // restoring the old device configuration...
    if(mDesc != INVALID_DESC) {
        tcsetattr(mDesc, TCSANOW, &mOrigPortSettings);   
        LOG(LINFO)<<"device configuration is restored..."<<std::endl;
    }
}

bool
SerialDevice::open(const SerialDeviceConfig& cfg)
{
	mDesc = ::open(cfg.deviceName.c_str(), O_RDWR | O_NOCTTY);
	// if open is unsucessful
	if(mDesc == -1) {
		LOG(LERROR)<<"Unable to open "<<cfg.deviceName<<std::endl;
        return false;
	} else	{
		fcntl(mDesc, F_SETFL, 0);
		LOG(LINFO)<<"successfully opened "<<cfg.deviceName<<std::endl;
	}

    // now do the configuration
	struct termios portSettings;      // structure to store the port settings in
    tcgetattr(0, &portSettings);
    memcpy(&mOrigPortSettings, &portSettings, sizeof(mOrigPortSettings));
    // set baud rates
	cfsetispeed(&portSettings, cfg.baudRate);    	
    cfsetospeed(&portSettings, cfg.baudRate);
    cfmakeraw(&portSettings);
	// apply the settings to the port
	tcsetattr(mDesc, TCSANOW, &portSettings);   
    LOG(LINFO)<<"device is properly configured"<<std::endl;
 
	return true;


}

ssize_t
SerialDevice::write(const uint8_t* data, size_t size)
{
    if(mDesc == INVALID_DESC) {
        return 0;
    }
    ssize_t numWrite = ::write(mDesc, data, size); 
    if(numWrite != size) {
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
    if(numRead != size) {
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
}


