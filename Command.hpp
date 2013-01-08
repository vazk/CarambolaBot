#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <stdint.h>

struct Command
{
    enum { COMMAND_PACKET_LENGTH = 8 };
    enum Type 
    {
        CMD_ACK,
        CMD_RESET,
        CMD_DRIVE,
        CMD_QUIT
    };
    
    union Data
    {
        uint8_t raw[COMMAND_PACKET_LENGTH];

        uint8_t type;     

        struct DriveCmd 
        {
            uint8_t type;
            int16_t left;
            int16_t right;
        } drive;
    }; 

    Data data;
};


#endif //COMMANDS_HPP
