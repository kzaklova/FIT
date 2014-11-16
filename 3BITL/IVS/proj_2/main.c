/**************
 * Project 2 for IVS
 * Author: Katerina Zaklova, 2014
 **************/

/**
* @file main.c
*
* @brief Main class of the project
* @author Katerina Zaklova, xzaklo00
*/

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "down.h"
#include "parity.h"

/** Failure of any function */
#define FAILED -1

/** Help message for user */
const char *HELPMSG = "Projekt 2 do predmetu IVS\n"
"Autor: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz, 2014 \n"
"Program pocita paritu pro zadany soubor (lokalne, popr. si soubor stahne).\n"
"Pouziti: ./xzaklo00 filename\n"
"         ./xzaklo00 URL1 filename1 URL2 filename2\n";

/**
 * Main function
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return exit success or error code
 */
int main (int argc, char *argv[]) {
	int ret = 0;

	// display help message
	if ((strcmp("-h", argv[1]) == 0) || (strcmp("--help", argv[1]) == 0) || argc < 2) {
		printf ("%s", HELPMSG);
		return EXIT_SUCCESS;
	}

	// just one file
	if (argc == 2) {
			ret = CalculateParity(argv[1]);
			// parity wasn't calculated
			if (ret == -1) {
				fprintf(stderr, "Parity error.");
				return FAILED;
			}
			// parity value
			else {
				printf("%d\n", ret);
				return ret;
			}
	}

	// odd count of arguments, some URL or file is missing
	if ((argc % 2 == 0) && (argc > 2)) {
		fprintf(stderr, "Wrong arguments, rerun with option -h.");
		return FAILED;
	}

	// even count of arguments, let's process the files
	for (int i = 1; i < argc; i += 2) {
		if (!DownloadFile (argv[i], argv[i+1])) {
			ret = CalculateParity (argv[i+1]);
			// parity wasn't calculated
			if (ret == -1)
				fprintf(stderr, "Parity error.");
			// parity value
			else
				printf("%d\n", ret);
		}
		// file wasn't downloaded
		else {
			fprintf(stderr, "Cannot download file");
			return FAILED;
		}
	}



	return ret;
}
