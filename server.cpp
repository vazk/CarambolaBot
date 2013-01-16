#include "Logger.hpp"
#include "SocketManager.hpp"
#include "SerialDevice.hpp"
#include "DualMotorController.hpp"
#include "CommandManager.hpp"
#include "Utils.hpp"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

struct Bundle {
    DualMotorController* dmc;
    SocketManager* sm;
    int port;
};

void* listenerFunc(void* bndp)
{
    Bundle* bnd = (Bundle*)bndp;
    SocketManager* socketManager = bnd->sm;
    LOG(LINFO)<<"waiting for socket connection..."<<std::endl;
    socketManager->waitForConnection(bnd->port);
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
        if(now - lastActivity > 3000) {
            LOG(LWARNING)<<"WARN: nothing received within the last 2s, closing the connection..."<<std::endl;
            socketManager->close();
        }
        if(now - lastActivity > 800) {
            LOG(LWARNING)<<"WARN: nothing received within the last 300ms, stopping the robot..."<<std::endl;
            motorController->setSpeeds(0, 0);
        } 
    }
    return NULL;
}

void printUsage(const std::string& progName)
{
    std::cout << "Usage: " << std::endl;
    std::cout << progName << " -l <log-file> -p <server-port>" << std::endl;
}

int main(int argc, char **argv)
{
    const char* logFileName = "server.log";
    int port = 9999;
    int opt;
    opterr = 0;    

    while ((opt = getopt (argc, argv, "l:p:")) != -1) {
        switch(opt) {
            case 'l':
                logFileName = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                printUsage(argv[0]);
                return 1;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }


    // define and configure device
    SerialDevice::SerialDeviceConfig cfg;
    cfg.baudRate = SerialDevice::SerialDeviceConfig::BAUD_38400;
    cfg.deviceName = "/dev/ttyS0";

    SerialDevice device;
    device.open(cfg);

    std::ofstream logFile(logFileName);
    Logger::initialize(LINFO, &logFile);

    daemon(0,0);
    
    // instantiate the motor controller
    DualMotorController motorController(device);

    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_DEVICE_ID, 9);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_PWM_PARAMETER, 1);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR, 0);
    motorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SERIAL_TIMEOUT, 0);

    LOG(LINFO)<<"reading Qik error code: ["<<motorController.getErrors()<<"]"<<std::endl;
    LOG(LINFO)<<"reading Qik firmware version: ["<<motorController.getFirmwareVersion()<<"]"<<std::endl;

    // instantiate the socket manager
    SocketManager socketManager;

    // instantiate the command parser/manager
    CommandManager commandManager(socketManager, motorController);


    pthread_t watchdogThread, listenerThread;

    Bundle bnd;
    bnd.dmc = &motorController;
    bnd.sm = &socketManager;
    bnd.port = port;

    if(pthread_create(&watchdogThread, NULL, watchdogFunc, &bnd)) {
        LOG(LERROR)<<"failed to create the watchdog thread..."<<std::endl;
        return 1;
    }

    // start the main loop
    while(!commandManager.shouldQuit()) {

    	if(pthread_create(&listenerThread, NULL, listenerFunc, &bnd)) {
            LOG(LERROR)<<"failed to create the listening thread..."<<std::endl;
		    return 1;
    	}
    	if(pthread_join(listenerThread, NULL)) {
            LOG(LERROR)<<"failed to join the listening thread..."<<std::endl;
	    	return 2;
    	}

        if(socketManager.isConnected()) {
            LOG(LINFO)<<"socket created, starting the communication..."<<std::endl;
            // run the communication...
            commandManager.run();
        } else {
            sleep(1);
        }

    }

	return 0;
}
