#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "SocketDevice.hpp"
#include "DualMotorController.hpp"


class CommandManager
{
    enum State
    {
        STARTING,
        RUNNING,
        QUITTING,
    };

public:
    CommandManager(SocketDevice& sm, DualMotorController& dmc);
    bool run();
    bool shouldQuit();

private:
    State                mState;
    SocketDevice&        mSocketDevice;
    DualMotorController& mMotorController;
};

#endif //COMMAND_MANAGER_HPP
