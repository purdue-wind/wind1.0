// Logging utilities

#ifndef _LOGGER
#define _LOGGER

#include <stdio.h>

#define LOGLINE_SIZE    256

#ifndef _TRACE
#define _TRACE
#ifdef DEBUG
    #define TRACE(...) Logger::tracewrite(__FUNCTION__, __LINE__, __VA_ARGS__, NULL);
#else
    #define TRACE
#endif
#endif

class Logger
{
public:
    Logger() {};
    ~Logger(void) {};
    
    static size_t tracewrite(const char *function, unsigned int line, const char *format, ...);
    static void getTimeStamp(char *buffer, size_t bufSize);
};

#endif


