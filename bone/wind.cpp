// wind.cpp
//
// Main class for the wind project, currenly contains code to hook into the instrument classes to produce
// diagnostic output to display on the web.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "analoginstrument.h"
#include "windoutput.h"
#include "windoutput-http.h"

#define BUFSIZE 512
#define NUM_TEST_INST 7
#define UPDATE_INTERVAL 2

using namespace std;

void openInstruments(AnalogInstrument *instList[], int count);
void updateInstruments(AnalogInstrument *instList[], int count);
void closeInstruments(AnalogInstrument *instList[], int count);
void serializeOutputLine(char output[],
                         timeval *curTime, 
                         AnalogInstrument *instList[], 
                         int count, 
                         size_t *bytesOut); 


AnalogInstrument *gTestInstruments[NUM_TEST_INST];

void openInstruments(AnalogInstrument *instList[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        instList[i]->open();
}

void updateInstruments(AnalogInstrument *instList[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        instList[i]->update();
}

void closeInstruments(AnalogInstrument *instList[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        instList[i]->open();
}

// Todo: Place most of this into a method of the windmill's
// output interface. For now, main() is being used to test the
// AnalogInstrument class.

int main (void)
{
    WindOutput *output;
    WindOutput *outHttp; //HTTP *outHttp;
    timeval curTime;
    timeval updateTime;
    char txt[BUFSIZE];
    char timeStamp[BUFSIZE]; 
    size_t bytesRead;
    int i;

    gettimeofday(&updateTime, NULL);

    // Initialize output - use command line parameter to determine
    // if we want file or web based output
    outHttp = new WindOutput(102912, 102400); //new WindOutputHTTP(102912, 102400);

    output = (WindOutput*)outHttp;
    output->initialize();

    for (i = 0; i < NUM_TEST_INST; i++)
        gTestInstruments[i] = new AnalogInstrument(i);

    openInstruments(gTestInstruments, NUM_TEST_INST);

    while(1)
	{
        int i;

        gettimeofday(&curTime, NULL);
        updateInstruments(gTestInstruments, NUM_TEST_INST);
        txt[0] = 0;
        serializeOutputLine(txt, 
                            &curTime, 
                            gTestInstruments, 
                            NUM_TEST_INST,
                            &bytesRead);
         
        output->update(txt, bytesRead);
        
        // Update user
        if (curTime.tv_sec - updateTime.tv_sec >= UPDATE_INTERVAL)
        {
            size_t reportBytes;
            unsigned int reportUpdates;

            output->getStatus(&reportUpdates, &reportBytes);

            cout << reportUpdates << " data updates pushed with " <<
                 reportBytes << " total bytes.\n";

            gettimeofday(&updateTime, NULL);
        }

        usleep(2500);
	}

	closeInstruments(gTestInstruments, NUM_TEST_INST);

	return 0;
}


void serializeOutputLine(char output[],
                         timeval *curTime, 
                         AnalogInstrument *instList[], 
                         int count, 
                         size_t *bytesOut)
{
    size_t curBytes;
    char curString[BUFSIZE];
    int i;

    // Process timestamp (move to dedicated function later)
    sprintf(output, "%ld.%ld,", curTime->tv_sec, curTime->tv_usec / 1000);

    for (i = 0; i < count; i++)
    {
        instList[i]->serializeOutput(curString, &curBytes);
        if (i < count-1)
            strcat(curString, ",");
        strcat(output, curString);
    }
    strcat(output, "\n");

    *bytesOut = strlen(output);
}

