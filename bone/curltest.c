#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

int main(void)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.excessiveheadspace.com/tars/");
	res = curl_easy_perform(curl);
    	curl_easy_cleanup(curl);
    }
    return 0;
}
