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

WindOutput::WindOutput()
{
    this->updates = 0;
    this->bytesOut = 0;
    this->bufSize = 0;
    this->outBuffer = (char*)malloc(WINDOUTPUT_DEFAULT_BUFSIZE);
    this->outFile = NULL;
}

WindOutput::~WindOutput()
{
    if (this->outFile)
        fclose(this->outFile);
    free(this->outBuffer);
}

void WindOutput::initialize()
{
    this->openNextFile(); 
}

void WindOutput::update(const char *data, size_t bytes)
{
    // Flush if the buffer max will be exceeded by the next update
    if (bytes + this->bufSize > WINDOUTPUT_DEFAULT_MAX)
    {
        flushBuffer();
        this->openNextFile();
    }

    // Copy in new data
    memcpy(this->outBuffer + this->bufSize, data, bytes);
    this->bufSize += bytes;
    this->bytesOut += bytes;
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

