// windoutput-http.h
// 
// HTTP Output class for the wind project. Uses the libcurl library
// to post wind data to a website.

#ifndef _WINDOUTPUTHTTP
#define _WINDOUTPUTHTTP

#define MAX_URL 512

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include "windoutput.h"

class WindOutputHTTP : public WindOutput
{
public:
    WindOutputHTTP(size_t bufferSize, size_t maxBytes);
    virtual ~WindOutputHTTP(void);

    virtual void initialize(void);
    //virtual void update(const char *data, size_t bytes);
    //virtual void getStatus(unsigned int *updates, size_t *bytesOut);
    void setURL (const char *url);

protected:
/*
    unsigned int updates;
    size_t bytesOut;
    char *outBuffer;
    size_t bufSize;
*/
    virtual void flushBuffer(void);

private:
    CURL *curl;
    char url[MAX_URL];
};

#endif

