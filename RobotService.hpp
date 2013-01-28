#ifndef ROBOT_SERVICE_HPP
#define ROBOT_SERVICE_HPP

#include "SocketDevice.hpp"
#include "UARTDevice.hpp"
#include "DualMotorController.hpp"
#include "CommandManager.hpp"
#include "Utils.hpp"

class RobotService 
{
public:
    RobotService(UARTDevice& device, int port);
    ~RobotService();
    void run();

private:
    static void* listenerFunc(void* pthis);
    static void* watchdogFunc(void* pthis);

private:
    int mPort;
    UARTDevice          mUARTDevice;
    SocketDevice        mSocketDevice;
    DualMotorController mMotorController;
    CommandManager      mCommandManager;
};


#endif //ROBOT_SERVICE_HPP


