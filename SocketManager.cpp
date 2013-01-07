#include "SocketManager.hpp"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>


SocketManager::SocketManager()
 : mIsConnected(false)
{
}

bool
SocketManager::waitForConnection(int port)
{
    close();
    struct sockaddr_in servAddr, cliAddr;
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        std::cerr<<"ERROR: failed fo open socket"<<std::endl;
        return false;
    }
    int on = 1;
    if(setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        std::cerr<<"ERROR: failed fo set socket options"<<std::endl;
        return false;
    }
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(port);
    if (bind(mSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        std::cerr<<"ERROR: failed to bind the socket." <<std::endl;
        return false;
    }
    listen(mSocket,5);
    socklen_t clilen = sizeof(cliAddr);
    mConSocket = accept(mSocket, (struct sockaddr *) &cliAddr, &clilen);
    if (mConSocket < 0) {
        std::cerr<<"ERROR: failed to accept."<<std::endl;
        return false;
    }
    mIsConnected = true;
    return true;
}


bool
SocketManager::connectToServer(const std::string& ip, int port)
{
    close();
    mConSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mConSocket < 0) {
        std::cerr<<"ERROR: failed to open a socket."<<std::endl;
        return false;
    }
    struct hostent* server = gethostbyname(ip.c_str());
    if (server == NULL) {
        std::cerr<<"ERROR: no such host."<<std::endl;
        return false;
    }
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&servAddr.sin_addr.s_addr, server->h_length);
    servAddr.sin_port = htons(port);
    if (connect(mConSocket, (struct sockaddr *)& servAddr, sizeof(servAddr)) < 0)  {
        std::cerr<<"ERROR: failed to connect..."<<std::endl;
        return false;
    }
    mIsConnected = true;
    return true;
}

bool 
SocketManager::isConnected()
{
    return mIsConnected;
}

ssize_t
SocketManager::write(const uint8_t* data, size_t size)
{
    ssize_t numWrite = ::write(mConSocket, data, size);
    mIsConnected = numWrite < 0;
    return numWrite;
}

ssize_t
SocketManager::read(uint8_t* data, size_t size)
{
    ssize_t numRead = ::read(mConSocket, data, size);
    mIsConnected = numRead < 0;
    return numRead;
    
}

void 
SocketManager::close()
{
    ::close(mConSocket);
    ::close(mSocket);
    mIsConnected = false;
}
