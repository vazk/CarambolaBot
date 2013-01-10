#include "SocketManager.hpp"
#include "SerialDevice.hpp"
#include "DualMotorController.hpp"
#include "CommandManager.hpp"
#include "Utils.hpp"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

struct Bundle {
    DualMotorController* dmc;
    SocketManager* sm;
    pthread_t * listener;
};

void* listenerFunc(void* bndp)
{
    Bundle* bnd = (Bundle*)bndp;
    SocketManager* socketManager = bnd->sm;
    std::cout<<"INFO: waiting for socket connection..."<<std::endl;
    socketManager->waitForConnection(9999);
    return NULL;
}


void* watchdogFunc(void* bndp)
{
    Bundle* bnd = (Bundle*)bndp;
    DualMotorController* motorController = bnd->dmc;
    SocketManager* socketManager = bnd->sm;
    while(true) {
        usleep(100 * 1000);
        if(!socketManager->isConnected()) {
            continue;
        }
        uint32_t now = milliseconds();
        uint32_t lastActivity = socketManager->lastActivityMs();
        if(now - lastActivity > 2000) {
            std::cout<<"WARN: nothing received within the last 2s, closing the connection..."<<std::endl;
            socketManager->close();
        }
        if(now - lastActivity > 300) {
            std::cout<<"WARN: nothing received within the last 300ms, stopping the robot..."<<std::endl;
            motorController->setSpeeds(0, 0);
        } 
    }
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


    pthread_t watchdogThread, listenerThread;

    Bundle bnd;
    bnd.dmc = &motorController;
    bnd.sm = &socketManager;
    bnd.listener = &listenerThread;


    if(pthread_create(&watchdogThread, NULL, watchdogFunc, &bnd)) {
        std::cerr<<"ERROR: failed to create the watchdog thread..."<<std::endl;
        return 1;
    }

    // start the main loop
    while(!commandManager.shouldQuit()) {

    	if(pthread_create(&listenerThread, NULL, listenerFunc, &bnd)) {
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
            sleep(1);
        }

    }

	return 0;

}
