// windoutput.cpp
// 
// Output base class for the wind project. Subclasses should include
// www or streaming output, depending on requirements.

#ifndef _WINDOUTPUT
#define _WINDOUTPUT

#include <pthread.h>
#include <stddef.h>
#include <cstdio>

class WindOutput
{
public:
    WindOutput(size_t bufferSize, size_t maxBytes);
    virtual ~WindOutput(void);

    virtual void initialize(void);
    virtual void update(const char *data, size_t bytes);
    virtual void getStatus(unsigned int *updates, size_t *bytesOut);

    // Thread sync
    void lock(void);
    void unlock(void);
    void notifyBufferFull(void);

protected:
    unsigned int updates;
    size_t bytesOut;
    char *outBuffer;
    size_t bufSize;
    size_t maxBytes;
    pthread_t thread;
    pthread_mutex_t bufMutex;
    pthread_mutex_t waitMutex;
    pthread_cond_t flushCond;
    bool threadDone;

    void initThreading(void);
    virtual void flushBuffer(void);
    static void *threadEntry(void *param);

private:
    FILE *outFile;

    void openNextFile(void);
};

#endif

