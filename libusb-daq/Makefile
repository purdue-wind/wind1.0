// Makefile for mcc-daq/libusb project

CC = g++ -g
cc = gcc
LDFLAGS = -lusb-1.0
CFLAGS  = 

all: mcc-daq

test: mcc-daq.o
	$(cc) -o mcc-daq $(CFLAGS) mcc-daq.o $(LDFLAGS)

test.o: mcc-daq.c
	$(CC) -c $(CFLAGS) $< $(LDFLAGS)

clean:
	rm *.o


