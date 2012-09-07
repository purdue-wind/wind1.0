// i2cinstrument.cpp
// 
// Superclass for i2c instruments.

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "instrument.h"
#include "i2cinstrument.h"

using namespace std;

i2cInstrument::i2cInstrument(void)
{
    this->streamHandle = NULL;
}

i2cInstrument::~i2cInstrument()
{
    if (this->streamHandle)
        this->close();
}

// Perform all setup necessary to begin reading from the instrument.
void i2cInstrument::open(void)
{
#ifdef NOBONE
    // Setup for simulation mode
#endif
    // Setup for the actual beaglebone
}

void i2cInstrument::close(void)
{
#ifdef NOBONE
    // Teardown for the simulation mode
#endif
    // Teardown for the actual beaglebone
}

void i2cInstrument::update(void)
{

#ifdef NOBONE
    this->updateSimulated();
    return;
#endif

    if (!this->streamHandle)
        return;

    // Perform polling of the instrument's value(s). Store to member data.
    // When serializeOutput is called, this data will be serialized to text.
}

void i2cInstrument::serializeOutput(char *buffer, size_t *bytes)
{
    // At the end of the function call, buffer contains a comma-separated
    // string of values and bytes contains then number of bytes in
    // buffer, including the null terminator.
}

void i2cInstrument::updateSimulated()
{
    // Perform any updates needed for simulation mode.
}
