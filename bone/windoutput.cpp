// windoutput.cpp
// 
// Output base class for the wind project. Subclasses should include
// www or streaming output, depending on requirements.

#include <cstring>
#include <cstdlib>
#include "windoutput.h"

#define WINDOUTPUT_DEFAULT_BUFSIZE  102912
#define WINDOUTPUT_DEFAULT_MAX      102400
#define WINDOUTPUT_DEFAULT_FILE     "output/windout%d.txt"

WindOutput::WindOutput(size_t bufferSize, size_t maxBytes)
{
    this->updates = 0;
    this->bytesOut = 0;
    this->bufSize = 0;
    this->maxBytes = maxBytes;
    this->outBuffer = (char*)malloc(bufferSize);
    this->outFile = NULL;
}

WindOutput::~WindOutput()
{
    if (this->outFile)
        fclose(this->outFile);
    free(this->outBuffer);
}

// Initialize the thread and mutex(es) that the
// output object will be using for asynchronous
// I/O operations.
void WindOutput::initThreading()
{
    int err;

    // Initialize pthread mutexes and condition(s)
    this->bufMutex = PTHREAD_MUTEX_INITIALIZER;    
    this->waitMutex = PTHREAD_MUTEX_INITIALIZER;
    this->flushCond = PTHREAD_COND_INITIALIZER;

    err = pthread_create(&this->thread, 
                         NULL, 
                         WindOutput::threadEntry, 
                         (void*)this);

    if (err != 0)
    {
        // Todo: Handle errors
    }
}

void WindOutput::initialize()
{
    this->openNextFile();  // Open file
    this->initThreading(); // Start thread 
}

void WindOutput::update(const char *data, size_t bytes)
{
    this->lock(); // Make threadsafe

    // Flush if the buffer max will be exceeded by the next update
    if (bytes + this->bufSize > this->maxBytes)
    {
        this->notifyBufferFull();
        this->openNextFile();
    }

    // Copy in new data
    memcpy(this->outBuffer + this->bufSize, data, bytes);
    this->bufSize += bytes;
    this->bytesOut += bytes;

    this->unlock();
}

void WindOutput::getStatus(unsigned int *updates, size_t *bytesOut)
{
    *updates = this->updates;
    *bytesOut = this->bytesOut;
}

void WindOutput::flushBuffer(void)
{
    size_t bytesWritten;

    // Default flush to file
    bytesWritten = fwrite(this->outBuffer, 1, this->bufSize, this->outFile);
    this->bufSize = 0;
    this->updates++;
}

void WindOutput::openNextFile(void)
{
    char fileName[256];
    
    if (this->outFile)
        fclose(this->outFile);

    sprintf(fileName, WINDOUTPUT_DEFAULT_FILE, this->updates);
    this->outFile = fopen(fileName, "w");
}

// Thread synchronization
void WindOutput::lock()
{
    pthread_mutex_lock(&this->bufMutex);
}

void WindOutput::unlock()
{
    pthread_mutex_unlock(&this->bufMutex);
}

void WindOutput::notifyBufferFull()
{
    pthread_cond_signal(&this->flushCond);
}

void *WindOutput::threadEntry(void *param)
{
    WindOutput *me = (WindOutput*)param;

    me->lock();
    me->threadDone = false;
    me->unlock();

    while(1)
    {
        // Wait for buffer full signal
        me->lock();
        pthread_cond_wait(&me->flushCond, &me->bufMutex); // "Virtual" unlock
        //...but it exits the wait condition locked.

        me->flushBuffer(); // Perform a buffer flush
        me->unlock();
       
        // Exit thread if "done" flag is set. 
        if(me->threadDone)
            return NULL;
    }
}
