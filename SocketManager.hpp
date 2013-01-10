#ifndef SOCKET_MANAGER_HPP
#define SOCKET_MANAGER_HPP

#include <sys/types.h>
#include <stdint.h>
#include <string>

class SocketManager
{
public:
    SocketManager();

public: 
    bool waitForConnection(int port);    
    bool connectToServer(const std::string& ip, int port);
    bool isConnected();
    ssize_t write(const uint8_t* data, size_t size);
    ssize_t read(uint8_t* data, size_t size);
    void close();
    uint32_t lastActivityMs() const 
    { return mLastActivityMs; }

private:
    int mSocket;
    int mConSocket;
    int mPort;
    bool mIsConnected;
    uint32_t mLastActivityMs;
};

#endif //SOCKET_MANAGER_HPP
