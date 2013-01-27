#ifndef ROBOT_SERVICE_HPP
#define ROBOT_SERVICE_HPP

#include "SocketManager.hpp"
#include "SerialDevice.hpp"
#include "DualMotorController.hpp"
#include "CommandManager.hpp"
#include "Utils.hpp"

class RobotService 
{
public:
    RobotService(SerialDevice& device, int port);
    ~RobotService();
    void run();

private:
    static void* listenerFunc(void* pthis);
    static void* watchdogFunc(void* pthis);

private:
    int mPort;
    SerialDevice        mSerialDevice;
    SocketManager       mSocketManager;
    DualMotorController mMotorController;
    CommandManager      mCommandManager;
};




#endif //ROBOT_SERVICE_HPP


