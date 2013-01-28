#include "SocketDevice.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>

bool
SocketDevice::waitForConnection(int port)
{
    close();
    struct sockaddr_in servAddr, cliAddr;
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        LOG(LERROR)<<"failed fo open socket"<<std::endl;
        return false;
    }
    int on = 1;
    if(setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        LOG(LERROR)<<"failed fo set socket options"<<std::endl;
        close();
        return false;
    }
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(port);
    if (bind(mSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        LOG(LERROR)<<"failed to bind the socket." <<std::endl;
        close();
        return false;
    }
    listen(mSocket,5);
    socklen_t clilen = sizeof(cliAddr);
    mDesc = accept(mSocket, (struct sockaddr *) &cliAddr, &clilen);
    if (mDesc < 0) {
        close();
        LOG(LERROR)<<"failed to accept."<<std::endl;
        return false;
    }
    mLastActivityMs = milliseconds();
    return true;
}


bool
SocketDevice::connectToServer(const std::string& ip, int port)
{
    close();
    mDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (mDesc < 0) {
        close();
        LOG(LERROR)<<"failed to open a socket."<<std::endl;
        return false;
    }
    struct hostent* server = gethostbyname(ip.c_str());
    if (server == NULL) {
        close();
        LOG(LERROR)<<"no such host."<<std::endl;
        return false;
    }
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&servAddr.sin_addr.s_addr, server->h_length);
    servAddr.sin_port = htons(port);
    if (connect(mDesc, (struct sockaddr *)& servAddr, sizeof(servAddr)) < 0)  {
        close();
        LOG(LERROR)<<"failed to connect..."<<std::endl;
        return false;
    }
    mLastActivityMs = milliseconds();
    return true;
}

void 
SocketDevice::close()
{
    shutdown(mDesc, 2);
    SerialDevice::close();

    shutdown(mSocket, 2);
    ::close(mSocket);
}
