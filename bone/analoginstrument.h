// analoginstrument.cpp
// 
// Superclass for generic analog instruments. It reads a single Beaglebone 
// analog port. The basic analog instrument class reads voltage.

#ifndef _ANALOGINSTRUMENT
#define _ANALOGINSTRUMENT

#include "instrument.h"
#include <cstdio>

class AnalogInstrument : public Instrument
{
public:

    AnalogInstrument(int port);
    virtual ~AnalogInstrument(void);

    // Beaglebone port to use
    void setPort(int port);
    int getPort(void);

    virtual void open(void);
    virtual void close(void);
    virtual void update(void);

    virtual void serializeOutput(char *buffer, size_t *bytes);

    static double voltsFromTxt(char *txt);

protected:
    int port;
    FILE *streamHandle; 
    char outBuf[INST_BUF_MAX];
    double voltage;

private:
    char portDevice[INST_BUF_MAX];

    void updateSimulated(void);
};
#endif

