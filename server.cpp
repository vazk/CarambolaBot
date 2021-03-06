#include "RobotService.hpp"
#include "Logger.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <iostream>


void printUsage(const std::string& progName)
{
    std::cout << "Usage: " << std::endl;
    std::cout << progName << " -l <log-file> -p <server-port>" << std::endl;
}

int main(int argc, char **argv)
{
    const char* logFileName = "server.log";
    int port = 9999;
    int opt;
    opterr = 0;    

    while ((opt = getopt (argc, argv, "l:p:")) != -1) {
        switch(opt) {
            case 'l':
                logFileName = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                printUsage(argv[0]);
                return 1;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    // define and configure device
    UARTDevice::UARTDeviceConfig cfg;
    cfg.baudRate = UARTDevice::UARTDeviceConfig::BAUD_38400;
    cfg.deviceName = "/dev/ttyS0";

    UARTDevice serialDevice;
    serialDevice.open(cfg);

    std::ofstream logFile(logFileName);
    Logger::initialize(LDEBUG, &logFile);

    /// Disabling the daemonisation: since we now launch the process 
    /// through inittab using respawn, it can not be a daemon.
    //daemon(0,0);

    RobotService robot(serialDevice, port);
    robot.run();

    return 0;
}
