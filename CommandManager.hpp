#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "SocketManager.hpp"
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
    CommandManager(SocketManager& sm, DualMotorController& dmc);
    bool run();
    bool shouldQuit();

private:
    State                mState;
    SocketManager&       mSocketManager;
    DualMotorController& mMotorController;
};

#endif //COMMAND_MANAGER_HPP
