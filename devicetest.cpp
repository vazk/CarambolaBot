#include "SerialDevice.hpp"
#include "DualMotorController.hpp"
#include <iostream>

int main(void)
{ 
    SerialDevice::SerialDeviceConfig cfg;
    cfg.baudRate = SerialDevice::SerialDeviceConfig::BAUD_38400;
    cfg.dataBits = SerialDevice::SerialDeviceConfig::BITS_8;
    cfg.stopBits = SerialDevice::SerialDeviceConfig::STOP_BITS_1;
    cfg.parity   = SerialDevice::SerialDeviceConfig::PARITY_NONE;
    cfg.deviceName = "/dev/ttyUSB0";

    SerialDevice sdev;
    std::cout << "opening... " << sdev.open(cfg)<<std::endl;

	unsigned char data[] = { 0x02, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x01, 0x21, 0x03};
    std::cout << "writing... " << sdev.write(data, 13)<<std::endl;


    DualMotorController dmc(sdev);
    std::cout<<"firmware: "<<dmc.getFirmwareVersion()<<std::endl;
	
	return(0);
	
} //main
