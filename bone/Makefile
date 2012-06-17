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
        CFLAGS = -D NOBONE
    else
        CFLAGS =
    endif
endif

all: wind clean

wind: wind.o analoginstrument.o windoutput.o windoutput-http.o
	$(CC) -o $@ $^ $(LDFLAGS)
	mkdir -p output
        			
wind.o: wind.cpp
	$(CC) -c $(CFLAGS) $<

analoginstrument.o: analoginstrument.cpp analoginstrument.h instrument.h
	$(CC) -c $(CFLAGS) $<

windoutput.o: windoutput.cpp windoutput.h
	$(CC) -c $(CFLAGS) $<

windoutput-http.o: windoutput-http.cpp windoutput-http.h windoutput.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o

