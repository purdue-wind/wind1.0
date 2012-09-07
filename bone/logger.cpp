#include "logger.h"
#include <ctime>
#include "sys/time.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>

size_t Logger::tracewrite(const char *function, unsigned int line, const char *format, ...)
{
    char logContent[LOGLINE_SIZE];
    char timeStamp[LOGLINE_SIZE];
    size_t bytes;
    size_t contentSize;
    va_list va;    

    // Pull out params
    va_start(va, format);
    bytes = sprintf(logContent, format, va);
    va_end(va);
    contentSize = strlen(logContent);
    
    Logger::getTimeStamp(timeStamp, LOGLINE_SIZE);
    fprintf(stderr, "%s[%x:%4d] %s\n", timeStamp, (unsigned int)function, line, logContent);
    return strlen(timeStamp) + strlen(logContent) + 1;
}

void Logger::getTimeStamp(char *buffer, size_t bufSize)
{
    struct timeval curTime;
    struct tm * timeObj;
    
    gettimeofday(&curTime, NULL);
    timeObj = localtime(&curTime.tv_sec);
    strftime(buffer, bufSize, "%H:%M:%S", timeObj);
    sprintf(buffer, "%s.%03ld", buffer, curTime.tv_usec / 1000);
}
