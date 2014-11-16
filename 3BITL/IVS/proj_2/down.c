/**
* @file down.c
*
* @brief Definition of functions for downloading file
* @author Katerina Zaklova, xzaklo00
*/

#include <curl/curl.h>
#include <stdlib.h>

/**
 * Function for writing data
 */
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/**
 * Function for downloading URL into file
 *
 * @param link URL of file to download
 * @param filename downloaded file
 * @return exit success or error code
 */
int DownloadFile (char *link, char *filename) {
	// variables for libcurl
	FILE *myfile;
	CURL *curl;
	CURLcode result;

	// init libcurl
	curl = curl_easy_init();

	if (curl) {
		// set options for downloading
		curl_easy_setopt (curl, CURLOPT_URL, link);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		myfile = fopen(filename,"wb");
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, myfile);
		// download
		result = curl_easy_perform (curl);
		fclose(myfile);

		// file was downloaded successfully
		if (result != CURLE_OK)
			return EXIT_FAILURE;

		// clean the mess
		curl_easy_cleanup(curl);
	}

	return EXIT_SUCCESS;
}
