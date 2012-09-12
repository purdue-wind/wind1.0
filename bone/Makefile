# Makefile
#
# Makes the beaglebone portion of the wind project.

CC	= g++
LDFLAGS = -lcurl
CFLAGS  = 

# Check if we are building on a beaglebone or other architecture
# by testing whether or not the beaglebone's analog ports are present.

ifeq ($(FORCEBEAGLEBONE),)
    ifeq ($(wildcard /sys/devices/platform/tsc/ain0),)
        CFLAGS = -g -pthread -std=c++0x -D NOBONE -D DEBUG
    else
        CFLAGS = -pthread -std=c++0x
    endif
endif

all: wind clean

wind: wind.o logger.o analoginstrument.o i2cinstrument.o windoutput.o windoutput-http.o
	$(CC) -o $@ $^ $(LDFLAGS)
	mkdir -p output
        			
wind.o: wind.cpp
	$(CC) -c $(CFLAGS) $<

analoginstrument.o: analoginstrument.cpp analoginstrument.h instrument.h
	$(CC) -c $(CFLAGS) $<

i2cinstrument.o: i2cinstrument.cpp i2cinstrument.h instrument.h
	$(CC) -c $(CFLAGS) $<

windoutput.o: windoutput.cpp windoutput.h
	$(CC) -c $(CFLAGS) $<

windoutput-http.o: windoutput-http.cpp windoutput-http.h windoutput.h
	$(CC) -c $(CFLAGS) $<

logger.o: logger.cpp logger.h
	$(CC) -c $(CGFLAGS) $<

clean:
	rm *.o

