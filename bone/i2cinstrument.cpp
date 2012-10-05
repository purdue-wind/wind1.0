// i2cinstrument.cpp
// 
// Superclass for i2c instruments.
// Assumes serial communication through the XBee->FIO bridge.

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <sstream>

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

// XBee commands
#define XBEE_COMMAND_MODE "+++"
#define XBEE_ACK          "OK\r"
#define XBEE_CH           "ATCH"
#define XBEE_CL	          "ATCL"
#define XBEE_CN	          "ATCN" // Exit command mode
#define XBEE_SL	          "ATSL"
#define XBEE_SH	          "ATSH"


using namespace std;

i2cInstrument::i2cInstrument(void)
{
    this->streamHandle = NULL;
    this->fileDesc = -1;
}

i2cInstrument::~i2cInstrument()
{
    if (this->fileDesc > -1)
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
    printf("UART RX: %s", buf); // TEMP

    bytesRead = fread(buf, 1, 1024, uartTXFD);
    buf[bytesRead] = 0;
    printf("UART TX: %s", buf); // TEMP

    fclose(uartRXFD);
    fclose(uartTXFD);

    // Open the serial device
    ttyFD = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ttyFD == -1)
    {
        printf("Error opening tty.\n");
       // TODO: Handle error.
    } else 
    {
	fcntl(ttyFD, F_SETFL, 0);
        cfsetispeed(&ttyOptions, B57600);
        cfsetospeed(&ttyOptions, B57600);

	ttyOptions.c_cflag |= (CLOCAL | CREAD);
	ttyOptions.c_cflag &= ~PARENB;
	ttyOptions.c_cflag &= ~CSTOPB;
	ttyOptions.c_cflag &= ~CSIZE;
	ttyOptions.c_cflag |= CS8;

	ttyOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	ttyOptions.c_iflag &= (IXON | IXOFF | IXANY);
	ttyOptions.c_oflag &= ~OPOST;
	ttyOptions.c_cc[VMIN] = 0;
	ttyOptions.c_cc[VTIME] = 10;
        tcsetattr(ttyFD, TCSANOW, &ttyOptions);

        printf("Initialized TTY\n");
	this->fileDesc = ttyFD;
    	checkXBeeConnection();
    } 
}

size_t i2cInstrument::readFD(void *ptr, size_t max)
{
    char *d = (char*)ptr;
    size_t total = 0;
    size_t bytes = 0;

    while (total < max - 1 
           && (bytes = read(fileDesc, d, 1)) > 0)
    {
        total += bytes;
        d += bytes;
    }
    *d = 0;
   
    return total; 
}

int i2cInstrument::readXBeeAck(void)
{
    char buf[256];
    size_t bytes;
    int ok = 0;

    if (readFD(buf, 256))
    {
        if(!strcmp(buf, XBEE_ACK)) {
	    ok = 1;
	}	
    }

    return ok;
}

int i2cInstrument::checkXBeeConnection(void)
{
    char out[256];
    char buf[256];
    unsigned long serial;
    int bytesOut, bytesIn;

    printf("Checking XBee serial number...\n");
    fflush(stdout);
    strcpy(out, XBEE_COMMAND_MODE);
    bytesOut = write(fileDesc, out, 3);
    sleep(1);
    if (!readXBeeAck())
    {
        return 1;
        // Handle error
    }
    strcpy(out, XBEE_SH);
    strcat(out, "\r");
    bytesOut = write(fileDesc, out, strlen(out));
    bytesIn = readFD(buf, 256);
    if (bytesIn > 0)
    	printf("%s\n", buf);
    
    strcpy(out, XBEE_SL);
    strcat(out, "\r");

    bytesOut = write(fileDesc, out, strlen(out));
    bytesIn = readFD(buf, 256);
    if (bytesIn > 0)
	printf("%s\n", buf);

    strcpy(out, XBEE_CN);
    strcat(out, "\r");
    bytesOut = write(fileDesc, out, strlen(out));
    if (!readXBeeAck())
    {
        return 1;
        // Handle error
    }
    
    bytesOut = write(fileDesc, "echo", 4);
    bytesIn = readFD(buf, 5);
    if (bytesIn > 0)
	  printf("%s\n", buf);  

    return 0;    
}

void i2cInstrument::closeInst(void)
{
#ifdef NOBONE
    // Teardown for the simulation mode
#endif
    // Teardown for the actual beaglebone
    close(this->fileDesc); // Close serial connection to local XBee
    this->fileDesc = -1;
}

void i2cInstrument::update(void)
{

#ifdef NOBONE
    this->updateSimulated();
    return;
#endif

    if (this->fileDesc < 0)
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
