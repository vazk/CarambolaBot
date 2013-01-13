#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <pthread.h>
#include <sstream>
#include <fstream>

enum LogLevel {
        LNONE,
        LERROR, 
        LWARNING, 
        LINFO, 
        LDEBUG
};

// this should be in sync with LogLevel. 
// TBD: find a simple+proper way of doing this...
const std::string LogLevelNames[] = {
        "N",
        "E", 
        "W", 
        "I", 
        "D"
};

class Logger
{
public:
    Logger();
    ~Logger();
    std::ostringstream& os(LogLevel level);
    
    static LogLevel level(); 
    static bool isInitialized();
    static void initialize(LogLevel level,
                           std::ofstream* logFile);

private:
    std::ostringstream     mBuffer;
    // static properties
    static LogLevel        sLogLevel;
    static pthread_mutex_t sMutex;
    static bool	           sInitialized;
    static std::ofstream*  sLogFile;
};

#define LOG(llevel) \
    if (llevel > Logger::level() && Logger::isInitialized()); \
    else Logger().os(llevel)

#endif
