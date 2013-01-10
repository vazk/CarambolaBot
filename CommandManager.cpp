#include "CommandManager.hpp"
#include "Command.hpp"
#include <iostream>

CommandManager::CommandManager(SocketManager& sm, DualMotorController& dmc)
 : mState(STARTING),
   mSocketManager(sm),
   mMotorController(dmc)
{
}

bool
CommandManager::run()
{
    mState = RUNNING;
    Command cmd;

    while(mState == RUNNING) {
        int n = mSocketManager.read(cmd.data.raw, Command::COMMAND_PACKET_LENGTH);
        if (n < 0) {
            std::cerr<<"ERROR: failed to read from socket."<<std::endl;
            return false;
        }
        if (n != Command::COMMAND_PACKET_LENGTH) {
            std::cerr<<"ERROR: failed to read complete packet [" << n <<" bytes]."<<std::endl;
            return false;
        }

        switch(cmd.data.type) {
            case Command::CMD_ACK:
                std::cout<<"INFO: CMD [ack]"<<std::endl;
                break;
            case Command::CMD_RESET:
                //std::cout<<"INFO: CMD [reset]"<<std::endl;
                mMotorController.setSpeeds(0, 0);
                break;
            case Command::CMD_DRIVE:
                {
                    //std::cout<<"INFO CMD [drive]"<<std::endl;
                    uint8_t err = mMotorController.getErrors();
                    if(err) {
                        std::cout<<"ERROR: DualMotorController error: "<<err<<std::endl;
                    }
                    mMotorController.setSpeeds(-cmd.data.drive.left, cmd.data.drive.right);
                }
                break;
            case Command::CMD_QUIT:
                mState = QUITTING;
                //std::cout<<"INFO: CMD [quit]"<<std::endl;
                mMotorController.setSpeeds(0, 0);
                break;
            default:
                std::cout<<"INFO: CMD [invalid]"<<std::endl;
        }
    }
    mSocketManager.close();
    return true;
}

bool
CommandManager::shouldQuit()
{
    return mState == QUITTING;
}

