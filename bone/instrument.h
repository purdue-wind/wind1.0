// instrument.h
// 
// Instrumentation base class for the wind project

#ifndef _INSTRUMENT
#define _INSTRUMENT

#include <stddef.h>
#include <stdio.h>

#define INST_BUF_MAX    256

class Instrument
{
public:
    Instrument() {}
    ~Instrument(void) {}

    virtual void open(void) {}
    virtual void close(void) {}
    virtual void update(void) {}

    virtual void serializeOutput(char *buffer, size_t *bytes) {}

};

#endif


