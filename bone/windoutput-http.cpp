// windoutput-http.cpp
// 
// HTTP output class for the wind project. Uses the libcurl library
// to post wind data to a website.

#include <cstdlib>
#include <cmath>
#include <curl/curl.h>
#include "windoutput-http.h"

#define WINDOUTPUT_DEFAULT_BUFSIZE  102912
#define WINDOUTPUT_DEFAULT_MAX      102400
//#define WINDOUTPUT_DEFAULT_FILE     "output/windout%d.txt"

#define WINDOUTPUT_DEFAULT_URL  "http://sslab00.cs.purdue.edu:8081/test2.php"

WindOutputHTTP::WindOutputHTTP()
{
    this->updates = 0;
    this->bytesOut = 0;
    this->bufSize = 0;
    this->outBuffer = (char*)malloc(WINDOUTPUT_DEFAULT_BUFSIZE);
    this->curl = NULL;
    strcpy(this->url, WINDOUTPUT_DEFAULT_URL);
}

WindOutputHTTP::~WindOutputHTTP()
{
    curl_easy_cleanup(this->curl);
}

void WindOutputHTTP::initialize()
{
    // Init curl (easy mode for now)
    curl_global_init(CURL_GLOBAL_ALL);
    this->curl = curl_easy_init();
}

void WindOutputHTTP::update(const char *data, size_t bytes)
{
    //printf("test %s %d", data, this->bufSize);
    // Flush if the buffer max will be exceeded by the next update.
    // For HTTP we use effectively the same technique as the file-based
    // method, except we flush to the website and not a file.
    if (bytes + this->bufSize > WINDOUTPUT_DEFAULT_MAX)
    {
        flushBuffer();
    }

    // Copy in new data
    memcpy(this->outBuffer + this->bufSize, data, bytes);
    this->bufSize += bytes;
    this->bytesOut += bytes;
}

/*
void WindOutput::getStatus(unsigned int *updates, size_t *bytesOut)
{
    *updates = this->updates;
    *bytesOut = this->bytesOut;
}
*/

void WindOutputHTTP::flushBuffer(void)
{
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *postend = NULL;
    CURLcode res;
    char updateNoStr[256];

    sprintf(updateNoStr, "%d", this->updates);

    printf("Bytes: %d", this->bufSize);
    
    // Flush to HTTP 
    curl_formadd(&formpost, &postend, 
                 CURLFORM_COPYNAME, 
                 "update",
                 CURLFORM_COPYCONTENTS, 
                 updateNoStr,
                 CURLFORM_END);

    curl_formadd(&formpost, &postend, 
                 CURLFORM_COPYNAME, "output",
                 CURLFORM_COPYCONTENTS, this->outBuffer,
                 CURLFORM_CONTENTSLENGTH, this->bufSize,
                 CURLFORM_END);

    curl_easy_setopt(this->curl, CURLOPT_URL, this->url);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    
    // Execute
    res = curl_easy_perform(curl);

    // Clean up
    curl_formfree(formpost);

    this->bufSize = 0;
    this->updates++;
}

