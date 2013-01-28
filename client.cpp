#include "Logger.hpp"
#include "Command.hpp"
#include "SocketDevice.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    std::ofstream logFile("client.log");
    Logger::initialize(LERROR, &logFile);


    SocketDevice socketDevice;
    if(socketDevice.connectToServer("192.168.2.9", 9999) == false) {
        return 1;
    };

    while(true) {
        Command cmd;
        char ch;
        std::cin>>ch;
        switch (ch) {
            case 'q' :  
                cmd.data.type = Command::CMD_QUIT;
                break;
            case 'd' :  
                cmd.data.type = Command::CMD_DRIVE;
                cmd.data.drive.left = 50;
                cmd.data.drive.right = 50;
                break;
            case 's' :  
                cmd.data.type = Command::CMD_DRIVE;
                cmd.data.drive.left = 0;
                cmd.data.drive.right = 0;
                break;
            case 'o' :  
                cmd.data.type = Command::CMD_DRIVE;
                cmd.data.drive.left = 100;
                cmd.data.drive.right = -100;
                break;
            case 'e' :  
                cmd.data.type = Command::CMD_ACK;
                break;
            case 'r' :  
                cmd.data.type = Command::CMD_RESET;
                break;
            default : 
                cmd.data.type = -1;
        }
        LOG(LINFO)<<"received command: "<< (char)cmd.data.type <<", executing..."<<std::endl;
        socketDevice.write(cmd.data.raw, Command::COMMAND_PACKET_LENGTH);
    }
    return 0;
}
