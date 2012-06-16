// analoginstrument.cpp
// 
// Superclass for generic analog instruments. It from a single Beaglebone analog port.
// The basic analog instrument class reads voltage.

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "instrument.h"
#include "analoginstrument.h"

#define MAX_VOLTS 1.8
#define MAX_DEV 4096
#define ANALOG_DEVICE_PATH "/sys/devices/platform/tsc/ain"

using namespace std;

AnalogInstrument::AnalogInstrument(int port)
{
    this->streamHandle = NULL;
    this->setPort(port); 
}

AnalogInstrument::~AnalogInstrument()
{
    if (this->streamHandle)
        this->close();
}

void AnalogInstrument::setPort(int port)
{
#ifdef NOBONE
    // Do not switch an open port
    if (this->streamHandle)
        return;
#endif

    this->port = port;
    sprintf(this->portDevice, "%s%d", ANALOG_DEVICE_PATH, port+1);
}

int AnalogInstrument::getPort(void)
{
    return this->port;
}

void AnalogInstrument::open(void)
{
#ifdef NOBONE
    this->streamHandle = fopen(this->portDevice, "rd");
#endif
}

void AnalogInstrument::close(void)
{
#ifdef NOBONE
    fclose(this->streamHandle);
#endif
    this->streamHandle = NULL;
}

void AnalogInstrument::update(void)
{
    int bytesRead;

#ifdef NOBONE
    this->updateSimulated();
    return;
#endif

    if (!this->streamHandle)
        return;

    bytesRead = fread(this->outBuf, 1, INST_BUF_MAX , this->streamHandle);
    if (bytesRead != EOF)
    {
        this->outBuf[bytesRead] = 0;
        this->voltage = AnalogInstrument::voltsFromTxt(this->outBuf);
        fseek(this->streamHandle, 0, 0);
    }
}

void AnalogInstrument::serializeOutput(char *buffer, size_t *bytes)
{
    sprintf(buffer, "%.3f", this->voltage);
    *bytes = strlen(buffer) + 1;
}

double AnalogInstrument::voltsFromTxt(char *txt)
{
    return ((double)atoi(txt) / (double)MAX_DEV) * MAX_VOLTS;
}

void AnalogInstrument::updateSimulated()
{
    this->voltage = .1 * this->port;
}
