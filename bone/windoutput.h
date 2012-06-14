// windoutput.cpp
// 
// Output base class for the wind project. Subclasses should include
// www or streaming output, depending on requirements.

#ifndef _WINDOUTPUT
#define _WINDOUTPUT

#include <stddef.h>
#include <cstdio>

class WindOutput
{
public:
    WindOutput();
    virtual ~WindOutput(void);

    virtual void initialize(void);
    virtual void update(const char *data, size_t bytes);
    virtual void getStatus(unsigned int *updates, size_t *bytesOut);

protected:
    unsigned int updates;
    size_t bytesOut;
    char *outBuffer;
    size_t bufSize;

    virtual void flushBuffer(void);


private:
    FILE *outFile;

    void openNextFile(void);
};

#endif

