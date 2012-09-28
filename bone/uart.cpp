// UART test
// For i2cinstrument.cpp

#include <stdio.h>
#include <fcntl.h>
#include "i2cinstrument.h"

int main(void)
{
    i2cInstrument inst;
    inst.openInst();
    inst.closeInst();
}
