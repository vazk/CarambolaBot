#include "RobotService.hpp"
#include "Logger.hpp"
#include <pthread.h>
#include <unistd.h>


RobotService::RobotService(SerialDevice& device, int port)
 : mPort(port),
   mSerialDevice(device),
   mMotorController(mSerialDevice),
   mCommandManager(mSocketManager, mMotorController)
{
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_DEVICE_ID, 9);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_PWM_PARAMETER, 1);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR, 0);
    mMotorController.setConfigurationParameter(DualMotorController::QIK_CONFIG_SERIAL_TIMEOUT, 0);

    LOG(LINFO)<<"reading Qik error code: ["<<mMotorController.getErrors()<<"]"<<std::endl;
    LOG(LINFO)<<"reading Qik firmware version: ["<<mMotorController.getFirmwareVersion()<<"]"<<std::endl;

}

RobotService::~RobotService() 
{
    mSerialDevice.close();
}

void 
RobotService::run()
{
    pthread_t watchdogThread, listenerThread;

    if(pthread_create(&watchdogThread, NULL, watchdogFunc, this)) {
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
        if(mSocketManager.isConnected()) {
            LOG(LINFO)<<"socket created, starting the communication..."<<std::endl;
            // run the communication...
            mCommandManager.run();
        } else {
            sleep(1);
        }
    }
}

void* 
RobotService::listenerFunc(void* pthis)
{
    RobotService* robot = (RobotService*)pthis;
    SocketManager& socketManager = robot->mSocketManager;
    LOG(LINFO)<<"waiting for socket connection..."<<std::endl;
    socketManager.waitForConnection(robot->mPort);
    return NULL;
}


void* 
RobotService::watchdogFunc(void* pthis)
{
RobotService* robot = (RobotService*)pthis;
    DualMotorController& motorController = robot->mMotorController;
    SocketManager& socketManager = robot->mSocketManager;
    while(true) {
        usleep(100 * 1000);
        if(!socketManager.isConnected()) {
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

