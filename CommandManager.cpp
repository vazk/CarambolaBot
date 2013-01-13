#include "CommandManager.hpp"
#include "Logger.hpp"
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
            LOG(LERROR)<<"ERROR: failed to read from socket."<<std::endl;
            return false;
        }
        if (n != Command::COMMAND_PACKET_LENGTH) {
            LOG(LERROR)<<"ERROR: failed to read complete packet [" << n <<" bytes]."<<std::endl;
            return false;
        }

        switch(cmd.data.type) {
            case Command::CMD_HEARTBEAT:
                LOG(LDEBUG)<<"INFO: CMD [heart beat]"<<std::endl;
                break;
            case Command::CMD_ACK:
                LOG(LDEBUG)<<"INFO: CMD [ack]"<<std::endl;
                break;
            case Command::CMD_RESET:
                //LOG(LDEBUG)<<"INFO: CMD [reset]"<<std::endl;
                mMotorController.setSpeeds(0, 0);
                break;
            case Command::CMD_DRIVE:
                {
                    LOG(LDEBUG)<<"INFO CMD [drive]"<<std::endl;
                    uint8_t err = mMotorController.getErrors();
                    if(err) {
                        LOG(LERROR)<<"ERROR: DualMotorController error: "<<err<<std::endl;
                    }
                    mMotorController.setSpeeds(-cmd.data.drive.left, cmd.data.drive.right);
                }
                break;
            case Command::CMD_QUIT:
                mState = QUITTING;
                LOG(LDEBUG)<<"INFO: CMD [quit]"<<std::endl;
                mMotorController.setSpeeds(0, 0);
                break;
            default:
                LOG(LDEBUG)<<"INFO: CMD [invalid]"<<std::endl;
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

