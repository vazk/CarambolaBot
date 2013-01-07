#include "SocketManager.hpp"
#include "SerialDevice.hpp"
#include "DualMotorController.hpp"
#include "CommandManager.hpp"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

void* listenConnection(void *smptr)
{
    SocketManager* socketManager = (SocketManager*)smptr;
    std::cout<<"INFO: waiting for socket connection..."<<std::endl;
    socketManager->waitForConnection(9999);
    return NULL;
}

int main()
{
    // define and configure device
    SerialDevice::SerialDeviceConfig cfg;
    cfg.baudRate = SerialDevice::SerialDeviceConfig::BAUD_38400;
    cfg.deviceName = "/dev/ttyS0";

    SerialDevice device;
    device.open(cfg);
    
    // instantiate the motor controller
    DualMotorController motorController(device);

    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_DEVICE_ID, 9);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_PWM_PARAMETER, 1);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR, 0);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SERIAL_TIMEOUT, 0);

    std::cout<<"INFO: reading Qik error code: ["<<motorController.getErrors()<<"]"<<std::endl;
    std::cout<<"INFO: reading Qik firmware version: ["<<motorController.getFirmwareVersion()<<"]"<<std::endl;

    // instantiate the socket manager
    SocketManager socketManager;

    // instantiate the command parser/manager
    CommandManager commandManager(socketManager, motorController);


    // start the main loop
    while(!commandManager.shouldQuit()) {
        pthread_t listenerThread;
    	if(pthread_create(&listenerThread, NULL, listenConnection, &socketManager)) {
            std::cerr<<"ERROR: failed to create the listening thread..."<<std::endl;
		    return 1;
    	}
    	if(pthread_join(listenerThread, NULL)) {
    		//fprintf(stderr, "Error joining thread\n");
            std::cerr<<"ERROR: failed to join the listening thread..."<<std::endl;
	    	return 2;
    	}

        if(socketManager.isConnected()) {
            std::cout<<"INFO: socket created, starting the communication..."<<std::endl;
            // run the communication...
            commandManager.run();
        } else {
            sleep(10);
        }

    }

	return 0;

}
