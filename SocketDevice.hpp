#ifndef SOCKET_DEVICE_HPP
#define SOCKET_DEVICE_HPP

#include "SerialDevice.hpp"
#include <sys/types.h>
#include <stdint.h>
#include <string>

class SocketDevice : public SerialDevice
{

public: 
    bool waitForConnection(int port);    
    bool connectToServer(const std::string& ip, int port);
    void close();

private:
    int mSocket;
    int mPort;
};

#endif //SOCKET_DEVICE_HPP
