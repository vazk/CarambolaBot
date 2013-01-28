#include "UARTDevice.hpp"
#include "Logger.hpp"
#include <string.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <iostream>


UARTDevice::~UARTDevice()
{
    close();
}

bool
UARTDevice::open(const UARTDeviceConfig& cfg)
{
    mDesc = ::open(cfg.deviceName.c_str(), O_RDWR);
    // if open is unsucessful
    if(mDesc == -1) {
        LOG(LERROR)<<"Unable to open "<<cfg.deviceName<<std::endl;
        return false;
    } else    {
        fcntl(mDesc, F_SETFL, 0);
        LOG(LINFO)<<"successfully opened "<<cfg.deviceName<<std::endl;
    }

    // now do the configuration
    struct termios portSettings;      // structure to store the port settings in
    tcgetattr(0, &portSettings);
    memcpy(&mOrigSettings, &portSettings, sizeof(mOrigSettings));
    // set baud rates
    cfsetispeed(&portSettings, cfg.baudRate);        
    cfsetospeed(&portSettings, cfg.baudRate);
    cfmakeraw(&portSettings);
    // apply the settings to the port
    tcsetattr(mDesc, TCSANOW, &portSettings);   
    LOG(LINFO)<<"device is properly configured"<<std::endl;
 
    return true;
}

void 
UARTDevice::close()
{
    // restoring the old device configuration...
    if(mDesc != INVALID_DESC) {
        tcsetattr(mDesc, TCSANOW, &mOrigSettings);   
        LOG(LINFO)<<"device configuration is restored..."<<std::endl;
    }
    SerialDevice::close();
}


