/**
* @file parity.c
*
* @brief Definition of functions for processing parity
* @author Katerina Zaklova, xzaklo00
*/

#include <stdio.h>
#include <stdlib.h>

/** Getting parity failed */
#define FAIL -1

/**
 * Function for calculating parity of file
 *
 * @param filename
 * @return parity or -1 for error
 */
 int CalculateParity (char *filename) {
	FILE *myfile;
	int x = EOF;
	int par = 0;

	// open file
	myfile = fopen(filename,"r");
	if(myfile == NULL) {
		return FAIL;
	}

	// read the file and make XOR
	while((x = fgetc(myfile)) != EOF)
		par ^= x;

	// close file
	fclose(myfile);

	// XOR every bit in the byte
	for (int i = 0; i < 7; i++)
		par ^= par>>1;

	par &= 1;

	return par;
 }
