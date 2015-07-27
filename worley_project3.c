/*
 * Brett Worley
 * CEG-4350-C01
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

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
	char ch;
	unsigned int program_size = 0;
	int num, i;

	/* Allocate the memory space for the y86 emulator */
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



	while( (ch = fgetc(fp)) != EOF) {
		
		if(isxdigit(ch)) {
			
			num = ascii_to_hex(ch);
			memory[program_size++] = num;
		}
	}

	for(i = 0; i < 30; i++) {
		printf("Memory[%x]: %x\n", i, memory[i]);
	}

	fclose(fp);
	free(memory);

	return EXIT_SUCCESS;
}


int ascii_to_hex(char c)
{

	int num = (int)c;
	
	if(num < 58 && num > 47) {
		return num - 48;
	}

	if(num < 103 && num > 96) {
		return num - 87;
	}
	
	return num;
}
