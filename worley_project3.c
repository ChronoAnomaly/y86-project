/*
 * Brett Worley
 * CEG-4350-C01
 *
*/

#include <stdio.h>
#include <stdlib.h>

/* max size of the memory space for the y86 emulator */
#define MEMSIZE (int)pow(2, 32)

unsigned int pc;	/* program counter */
unsigned int stack_limit;	/* smallest address that the stack can not go past */

int reg[8];

int main(int argc, char** argv)
{

	FILE* fp;


	if(argc < 2) {
		printf("Not enough arguments for the program to run correctly\n.");
		return EXIT_FAILURE;
	}

	if(argc > 2) {
		printf("Too many arguments for the program to run correctly\n.");
		return EXIT_FAILURE;
	}

	if( (fp = fopen(argv[1], "r")) == NULL) {
		printf("ERROR: The file you provided does not exisit.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
