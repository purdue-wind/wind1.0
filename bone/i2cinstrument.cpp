// i2cinstrument.cpp
// 
// Superclass for i2c instruments.
// Assumes serial communication to an XBee.

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <sys/stat.h>
#include "logger.h"
#include "instrument.h"
#include "i2cinstrument.h"

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
// UART4 is used
#define UART_RX_conf "/sys/kernel/debug/omap_mux/gpmc_wait0"
#define UART_TX_conf "/sys/kernel/debug/omap_mux/gpmc_wpn"
#define UART_RX_conf_val 26
#define UART_TX_conf_val 6

#define SERIAL_DEVICE "/dev/ttyO4"

using namespace std;

i2cInstrument::i2cInstrument(void)
{
    this->streamHandle = NULL;
}

i2cInstrument::~i2cInstrument()
{
    if (this->streamHandle)
        this->closeInst();
}

// Perform all setup necessary to begin reading from the instrument.
// In this case, we initialize the serial port and set port attributes.
void i2cInstrument::openInst(void)
{
#ifdef NOBONE
    // Setup for simulation mode
#endif
    char buf[1024];
    size_t bytesRead;

    // Setup for the actual beaglebone
    FILE *uartTXFD, *uartRXFD;
    int ttyFD;
    struct termios ttyOptions;

    // Configure UARTs
    uartRXFD = fopen(UART_RX_conf, "rw");
    fprintf(uartRXFD, "%d", UART_RX_conf_val);

    uartTXFD = fopen(UART_TX_conf, "rw");
    fprintf(uartTXFD, "%d", UART_TX_conf_val);
    
    fflush(uartRXFD);
    fflush(uartTXFD);

    // Dump config info for checking
    fseek(uartRXFD, 0, 0);
    fseek(uartTXFD, 0, 0);

    bytesRead = fread(buf, 1, 1024, uartRXFD);
    buf[bytesRead] = 0;
    printf("UART RX: %s", buf);

    bytesRead = fread(buf, 1, 1024, uartTXFD);
    buf[bytesRead] = 0;
    printf("UART TX: %s", buf);

    fclose(uartRXFD);
    fclose(uartTXFD);

    // Open the serial device
    ttyFD = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (ttyFD == -1)
    {
        printf("Error opening tty.\n");
       // TODO: Handle error.
    } else 
    {
        cfsetispeed(&ttyOptions, B9600);
        cfsetospeed(&ttyOptions, B9600);
        tcsetattr(ttyFD, TCSANOW, &ttyOptions);

        printf("Initialized TTY\n");
    }
}

void i2cInstrument::closeInst(void)
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
