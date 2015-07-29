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
int execute();
int valid_address(unsigned int addr);


/*######################################
 * Global variable space
 *######################################
*/

unsigned int pc;	/* program counter */
unsigned int stack_limit;	/* smallest address that the stack can not go past */

unsigned char* memory;

/* Registers
 *-------------------------------------
 * reg[0] %eax
 * reg[1] %ecx
 * reg[2] %edx
 * reg[3] %ebx
 * reg[4] %esp - stack pointer
 * reg[5] %ebp - stack base pointer
 * reg[6] %esi
 * reg[7] %edi
*/
int reg[8];

/* Condition codes
 * flags are ZF, SF, OF
 * ZF: zero
 * SF: Sign flag
 * OF: Overflow
*/
int flags[3];

typedef enum {AOK = 1, HLT, ADR, INS} status;

status programStatus;

int main(int argc, char** argv)
{

	FILE* fp;
	char ch;
	unsigned int program_size = 0;
	int instruct, i;

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

	instruct = i = 0;

	while( (ch = fgetc(fp)) != EOF) {
		
		if(isxdigit(ch)) {
			
			instruct = ascii_to_hex(ch) << 4;
		}
		
		if( (ch = fgetc(fp)) != EOF) {
			
			if(isxdigit(ch)) {
			
			instruct += ascii_to_hex(ch);
			memory[program_size++] = instruct;
		}
		}
	}

	printf("test 0x%02x\n", memory[0]);
	printf("test 0x%02x\n", memory[1]);
	printf("test 0x%02x\n", memory[2]);
	printf("test 0x%02x\n", memory[3]);
	printf("test 0x%02x\n", memory[4]);
	printf("test 0x%02x\n", memory[5]);
	stack_limit = program_size;

	execute();

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

int valid_address(unsigned int addr)
{

	if( (addr < 0x0) || (addr > MEMSIZE)) {
		programStatus = ADR;
		return 0;
	}
	
	return 1;
}

int execute()
{

	unsigned int op_code, address;

	/* set the registers to 0x0 to begin the emulator */
	reg[0] = reg[1] = reg[2] = reg[3] = reg[6] = reg[7] = 0x0;
	/* set the stack registers to the max location in memory */
	reg[3] = reg[4] = 0xFFFFFFFF;
	pc = 0x0;

	while(programStatus == AOK) {

	}
}
