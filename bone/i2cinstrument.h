// i2cinstrument.h
//
// Superclass for i2c instruments.

#ifndef _I2CINSTRUMENT
#define _I2CINSTRUMENT

#include "instrument.h"
#include <cstdio>

class i2cInstrument : public Instrument
{
public:

    i2cInstrument();
    virtual ~i2cInstrument(void);

    virtual void open(void);
    virtual void close(void);
    virtual void update(void);
    virtual void serializeOutput(char *buffer, size_t *bytes);

protected:
    FILE *streamHandle; 

private:

    void updateSimulated(void);
};
#endif

