CC		= g++
CFLAGS	=
LDFLAGS = -lcurl

all: wind clean

wind: wind.o analoginstrument.o windoutput.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
wind.o: wind.cpp
	$(CC) -c $(CFLAGS) $<

analoginstrument.o: analoginstrument.cpp analoginstrument.h instrument.h
	$(CC) -c $(CFLAGS) $<

windoutput.o: windoutput.cpp windoutput.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o

