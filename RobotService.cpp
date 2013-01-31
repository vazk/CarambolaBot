#include "RobotService.hpp"
#include "Logger.hpp"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/// CTOR. After initialization of all the members it starts the watchdog thread,
/// configures the motor and tries to read the error and version info. Because 
/// of an unknown issue sometimes it may get stuck at this point, the watchdog 
/// thread is here to quit the program if that happens.
RobotService::RobotService(UARTDevice& device, int port)
 : mIsInitialized(false),
   mPort(port),
   mUARTDevice(device),
   mMotorController(mUARTDevice),
   mCommandManager(mSocketDevice, mMotorController)
{
    pthread_t watchdogThread;
    if(pthread_create(&watchdogThread, NULL, watchdogFunc, this)) {
        LOG(LERROR)<<"failed to create the watchdog thread..."<<std::endl;
        return;
    }
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_DEVICE_ID, 9);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_PWM_PARAMETER, 1);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR, 0);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SERIAL_TIMEOUT, 0);

    LOG(LINFO)<<"reading Qik error code: ["<<mMotorController.getErrors()<<"]"<<std::endl;
    LOG(LINFO)<<"reading Qik firmware version: ["<<mMotorController.getFirmwareVersion()<<"]"<<std::endl;

}

RobotService::~RobotService() 
{
    mUARTDevice.close();
}


/// The main service loop consisting of:
///   - waiting and establishing the connection,
///   - servicing the connection
/// The loop breaks when the robot sends the appropriate command (the use case is 
/// not obvious for now...)
void 
RobotService::run()
{
    pthread_t keepaliveThread, listenerThread;

    if(pthread_create(&keepaliveThread, NULL, keepaliveFunc, this)) {
        LOG(LERROR)<<"failed to create the watchdog thread..."<<std::endl;
        return;
    }

    // start the main loop
    while(!mCommandManager.shouldQuit()) {
        if(pthread_create(&listenerThread, NULL, listenerFunc, this)) {
            LOG(LERROR)<<"failed to create the listening thread..."<<std::endl;
            return;
        }
        if(pthread_join(listenerThread, NULL)) {
            LOG(LERROR)<<"failed to join the listening thread..."<<std::endl;
            return;
        }
        if(mSocketDevice.isOpen()) {
            LOG(LINFO)<<"socket created, starting the communication..."<<std::endl;
            // run the communication...
            mCommandManager.run();
        } else {
            sleep(1);
        }
    }
}

/// This is the main function of the thread responsible for waiting for the socket
/// connection. It blocks until a connection is established or an error occured.
void* 
RobotService::listenerFunc(void* pthis)
{
    RobotService* robot = (RobotService*)pthis;
    SocketDevice& socketManager = robot->mSocketDevice;
    LOG(LINFO)<<"waiting for socket connection..."<<std::endl;
    robot->mIsInitialized= true;
    socketManager.waitForConnection(robot->mPort);
    return NULL;
}

/// This is the main function of the thread responsible for monitoring the con-
/// nection and stopping the robot or closing the the connection if the heartbeat
/// is not sent during the past appropriate period of time.
void* 
RobotService::keepaliveFunc(void* pthis)
{
    RobotService* robot = (RobotService*)pthis;
    DualMotorController& motorController = robot->mMotorController;
    SocketDevice& socketManager = robot->mSocketDevice;
    while(true) {
        usleep(100 * 1000);
        if(!socketManager.isOpen()) {
            continue;
        }
        uint32_t now = milliseconds();
        uint32_t lastActivity = socketManager.lastActivityMs();
        if(now - lastActivity > 3000) {
            LOG(LWARNING)<<"WARN: nothing received within the last 2s, closing the connection..."<<std::endl;
            socketManager.close();
        }
        if(now - lastActivity > 800) {
            LOG(LWARNING)<<"WARN: nothing received within the last 300ms, stopping the robot..."<<std::endl;
            motorController.setSpeeds(0, 0);
        } 
    }
    return NULL;
}

/// This is basically a hack that we need to fight the problem of getting stuck at 
/// reading from ttyS0 during initialization. There seem to be a weird problem 
/// either with the board or the OS: when started as a part of init.d/rc.d, the 
/// process once in a while hangs at the point where it tries to read the from 
/// the serial device. 
/// The way this hack works is it creates a new thread which waits few hundred ms
/// and then checks if the main process moved on or got stuck at reading. In case
/// it's stuck the thread quits the program. The program then get's restarted by 
/// by init service... 
void*
RobotService::watchdogFunc(void* pthis)
{
    usleep(200 * 1000);
    RobotService* robot = (RobotService*)pthis;
    if(!robot->mIsInitialized) {
        LOG(LERROR)<<"ERROR: the service has not been started, quitting..."<<std::endl;
        exit(0);
    }
    return NULL;
}
