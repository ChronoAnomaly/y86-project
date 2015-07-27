/*
 * Brett Worley
 * CEG-4350-C01
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* max size of the memory space for the y86 emulator */
#define MEMSIZE (int)pow(2, 32)

/*######################################
 * Function define space
 *######################################
*/

int ascii_to_hex(char c);

/*######################################
 * Global variable space
 *######################################
*/

unsigned int pc;	/* program counter */
unsigned int stack_limit;	/* smallest address that the stack can not go past */

unsigned char* memory;

int reg[8];

typedef enum {AOK = 1, HLT, ADR, INS, STC} status;

int main(int argc, char** argv)
{

	FILE* fp;
	
	memory = (unsigned char*)malloc(sizeof(unsigned char) * MEMSIZE);

	if(memory != NULL) {
		printf("Valid memory pointer.\n");
	}

	if(argc < 2) {
		printf("Not enough arguments for the program to run correctly\n.");
		return EXIT_FAILURE;
	}

	if(argc > 2) {
		printf("Too many arguments for the program to run correctly\n.");
		return EXIT_FAILURE;
	}

	if( (fp = fopen(argv[1], "r")) == NULL) {
		printf("ERROR: The file you provided does not exist.\n");
		return EXIT_FAILURE;
	}



	while() {

	}
	fclose(fp);
	free(memory);

	return EXIT_SUCCESS;
}


int ascii_to_hex(char c)
{

	int nugm = (int)c;
	
	if(num < 58 && num > 47) {
		return num - 48;
	}

	if(num < 103 && num > 96) {
		return num - 87;
	}
	
	return num;
}
