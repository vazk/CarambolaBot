#include "Logger.hpp"
#include <string>
#include <stdio.h>
#include <time.h>

Logger::Logger() 
{
}

Logger::~Logger()
{
    pthread_mutex_lock(&sMutex); 
    // do the initialization
    pthread_mutex_unlock(&sMutex); 
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

std::ostringstream& 
Logger::os(LogLevel level)
{
    mBuffer << currentDateTime() << " ["<< LogLevelNames[level] << "] ";
    return mBuffer;
}

LogLevel 
Logger::level() 
{
    return sLogLevel;
}
bool 
Logger::isInitialized() 
{
    return sInitialized;
}
void 
Logger::initialize(LogLevel level)
    
{
    sLogLevel = level;
    pthread_mutex_init(&sMutex, NULL);
    sInitialized = true;
}

LogLevel        Logger::sLogLevel;
pthread_mutex_t Logger::sMutex;
bool            Logger::sInitialized = false;
