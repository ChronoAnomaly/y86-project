/*
 * Brett Worley
 * CEG-4350-C01
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* max size of the memory space for the y86 emulator */
#define MEM_SIZE (int)pow(2, 32)

/*######################################
 * Function define space
 *######################################
*/

int ascii_to_hex(char c);
void execute();
int valid_address(unsigned int addr);
void print_output();
void rrmovl(int regA, int regB);
void irmovl(int val, int regA);
void mrmovl(int regA, int regB);
void opl(int function, int regA, int regB);
void jump(int function, unsigned int dest);
void cmov(int function, int regA, int regB);
void call(unsigned int dest);
void ret();
void push(int regA);
void pop(int regA);

/*######################################
 * Global variable space
 *######################################
*/

unsigned int pc;	/* program counter */
int steps;		/* number of steps the that emulator has taken */
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
	memory = (unsigned char*)malloc(sizeof(unsigned char) * MEM_SIZE);

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

	print_output();

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

	if( (addr < 0x0) || (addr > MEM_SIZE)) {
		programStatus = ADR;
		return 0;
	}
	
	return 1;
}


void execute()
{

	unsigned int op_code, address;

	/* set the registers to 0x0 to begin the emulator */
	reg[0] = reg[1] = reg[2] = reg[3] = reg[6] = reg[7] = 0x0;
	/* set the stack registers to the max location in memory */
	reg[3] = reg[4] = 0xFFFFFFFF;
	pc = 0x0;
	steps = 0;
	programStatus = AOK;

	while(programStatus == AOK) {

		steps++;
		op_code = memory[pc];
		/* TODO: remove after testing */
		printf("Testing op code 0x%02x\n", op_code);

		/* determine OP Code */
		if( (op_code & 0xff) == 0x10) {
			/* halt instrution */
			programStatus == HLT;
		} else if( (op_code & 0xff) == 0x00) {
			/* nop instruction, do nothing */
		} else if( (op_code & 0xff) == 0x30) {

		} else if( (op_code & 0xff) == 0x40) {

		} else if( (op_code & 0xff) == 0x50) {

		} else if( (op_code & 0xff) == 0x60) {

		} else if( (op_code & 0xff) == 0x70) {
		} else if( (op_code & 0xff) == 0x20) {
		} else if( (op_code & 0xff) == 0x80) {
		} else if( (op_code & 0xff) == 0x90) {
		} else if( (op_code & 0xff) == 0xa0) {
		} else if( (op_code & 0xff) == 0xb0) {
		} else {
			/* invalid instruction encountered */
			programStatus == INS;
		}




	}
}

void rrmovl(int regA, int regB)
{
}

void irmovl(int val, int regA)
{

	reg[regA] = val;
	printf("irmovl %d into reg[%x]\n", val, regA);
	pc += 6;
	printf("Reg = %x\n", reg[regA]);
}

void mrmovl(int regA, int regB)
{
}

void opl(int function, int regA, int regB)
{
}

void jump(int function, unsigned int dest)
{
}

void cmov(int function, int regA, int regB)
{
}

void call(unsigned int dest)
{
}

void ret()
{
}

void push(int regA)
{

	reg[4] -= 4;
	memory[reg[4]] = reg[regA];
}

void pop(int regA)
{
	
	reg[regA] = memory[reg[4]];
	reg[4] += 4;
}
void print_output()
{

	int i, flag;
	char str[4];
	
	if(programStatus == AOK) {
		strcpy(str, "AOK");
	} else if(programStatus == HLT) {
		strcpy(str, "HLT");
	} else if(programStatus == ADR) {
		strcpy(str, "ADR");
	} else if(programStatus == INS) {
		strcpy(str, "INS");
	}

	printf("Stopped in %d steps at PC - 0x%x.", steps, pc);
	printf("  Status '%s', CC Z=%d S=%d", str, flags[0], flags[1]);
	printf(" O=%d\n", flags[2]);
	printf("Changes to the registers: \n");
	
	flag = 0;
	/* print out changes made to the registers, if any */
	for(i = 0; i < 8; i++) {

	}
	
	if(flag == 0) {
		printf("None\n");
	}
	
}
