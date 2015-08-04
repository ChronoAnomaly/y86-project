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
void rmmovl(int regA, int regB, unsigned int dest);
void mrmovl(int regA, int regB, unsigned int dest);
void opl(int function, int regA, int regB);
void jump(int function, unsigned int dest);
void cmov(int function, int regA, int regB);
void call(unsigned int dest);
void set_flags(int function, unsigned int val1,
	 unsigned int val2, unsigned int result);
void ret();
void pushl(int regA);
void popl(int regA);

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
	int instruct;

	/* Allocate the memory space for the y86 emulator */
	memory = (unsigned char*)malloc(sizeof(unsigned char) * MEM_SIZE);

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

	instruct = 0;

	while( (ch = fgetc(fp)) != EOF) {
		
		if(isxdigit(ch)) {
			
			instruct = ascii_to_hex(ch) << 4;
	
			if( (ch = fgetc(fp)) != EOF) {
			
				if(isxdigit(ch)) {
			
					instruct += ascii_to_hex(ch);
printf("Reading hex as %02x into address memory[%08x]\n", instruct, program_size);
					memory[program_size++] = instruct;
				}
			}
		}
	}

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

	unsigned int op_code, address, value;
	int src, dest, function;

	/* set the registers to 0x0 to begin the emulator */
	reg[0] = reg[1] = reg[2] = reg[3] = reg[6] = reg[7] = 0x0;
	/* set the stack registers to the max location in memory */
	reg[3] = reg[4] = 0xFFFFFFFF;
	flags[0] =flags[1] = flags[2] = 0;
	pc = 0x0;
	steps = 0;
	programStatus = AOK;

	while(programStatus == AOK) {
printf("Flags are: %d %d %d\n", flags[0], flags[1], flags[2]);
		steps++;
		op_code = memory[pc];
		/* determine OP Code */
		if( (op_code & 0xff) == 0x00) {
			
			/* halt instruction */
			programStatus = HLT;

		} else if( (op_code & 0xff) == 0x10) {
			/* nop instruction, do nothing */
		} else if( (op_code & 0xff) == 0x30) {
printf("Instruction is %x%x%x%x%x%x\n",memory[pc],memory[pc+1],memory[pc+2],memory[pc+3],memory[pc+4],memory[pc+5]);
printf("mem[%08x]: %02x mem[%08x]: %02x mem[%08x]: %02x",pc,memory[pc],pc+1,memory[pc+1],pc+2,memory[pc+3]);
printf(" mem[%08x]: %02x mem[%08x]: %02x mem[%08x]: %02x\n",pc+3,memory[pc+3],pc+4,memory[pc+4],pc+5,memory[pc+5]);
			/* irmovl instruction */
			value = memory[pc + 2] |
				memory[pc + 3] << 8 |
				memory[pc + 4] << 16 |
				memory[pc + 5] << 24;
printf("Value of converted value is %x\n", value);
			dest = memory[pc + 1] & 0x0f;
			if( (dest == 4) || (dest == 5)) {
				programStatus = INS;
				break;
			}

			irmovl(value, dest);

		} else if( (op_code & 0xff) == 0x40) {
		
			/* rmmovl instruction */
			address = memory[pc + 2] |
				memory[pc + 3] << 8 |
				memory[pc + 4] << 16 |
				memory[pc + 5] << 24;
			
			if(!valid_address(address)) {
				programStatus = ADR;
				break;
			}

			src = (memory[pc + 1] & 0xf0) >> 4;
			dest = memory[pc + 1] & 0x0f;

			if( (dest == 4) || (dest == 5) || (src == 4) ||
			(src == 5)) {
				programStatus = INS;
				break;
			}

			rmmovl(src, dest, address);

		} else if( (op_code & 0xff) == 0x20) {
	
			/* rrmovl instruction */
			src = (memory[pc + 1] & 0xf0) >> 4;
			dest = memory[pc + 1] & 0x0f;

			if( (dest == 4) || (dest == 5) || (src == 4) ||
			(src == 5)) {
				programStatus = INS;
				break;
			}

			rrmovl(src, dest);


		} else if( (op_code & 0xff) == 0x50) {

			/* mrmovl instruction */
			address = memory[pc + 2] |
				memory[pc + 3] << 8 |
				memory[pc + 4] << 16 |
				memory[pc + 5] << 24;
			
			if(!valid_address(address)) {
				programStatus = ADR;
				break;
			}

			src = (memory[pc + 1] & 0xf0) >> 4;
			dest = memory[pc + 1] & 0x0f;

			if( (dest == 4) || (dest == 5) || (src == 4) ||
			(src == 5)) {
				programStatus = INS;
				break;
			}

			mrmovl(src, dest, address);

		} else if( (op_code & 0xff) == 0x60) {

			/* OPL instruction */
			function = memory[pc] & 0x0f;
			
			if(function < 0 || function > 3) {
				programStatus = INS;
				break;
			}

			src = (memory[pc + 1] & 0xf0) >> 4;
			dest = memory[pc + 1] & 0x0f;

			if( (dest == 4) || (dest == 5) || (src == 4) ||
			(src == 5)) {
				programStatus = INS;
				break;
			}

			opl(function, src, dest);

		} else if( (op_code & 0xff) == 0x70) {

			/* jump instruction */
			address = memory[pc + 2] |
				memory[pc + 3] << 8 |
				memory[pc + 4] << 16 |
				memory[pc + 5] << 24;
			
			if(!valid_address(address)) {
				programStatus = ADR;
				break;
			}

			function = memory[pc] & 0x0f;
	
			if(function < 0 || function > 6) {
				programStatus = INS;
				break;
			}

			jump(function, address);

		} else if( ((op_code & 0xff) >= 0x20)
			 && ((op_code & 0xff) <= 0x26)) {
		
			/* conditional move instruction */
			function = memory[pc] & 0x0f;
	
			if(function < 1 || function > 6) {
				programStatus = INS;
				break;
			}
			
			src = (memory[pc + 1] & 0xf0) >> 4;
			dest = memory[pc + 1] & 0x0f;

			if( (dest == 4) || (dest == 5) || (src == 4) ||
			(src == 5)) {
				programStatus = INS;
				break;
			}

			cmov(function, src, dest);

		} else if( (op_code & 0xff) == 0x80) {

			/* call instruction */
			address = memory[pc + 2] |
				memory[pc + 3] << 8 |
				memory[pc + 4] << 16 |
				memory[pc + 5] << 24;
			
			if(!valid_address(address)) {
				programStatus = ADR;
				break;
			}

			call(address);

		} else if( (op_code & 0xff) == 0x90) {

			/* ret instruction */
			ret();

		} else if( (op_code & 0xff) == 0xa0) {

			/* pushl instruction */
			dest = (memory[pc + 1] & 0xf0) >> 4;

			if( (dest == 4) || (dest == 5)) {
				programStatus = INS;
				break;
			}

		} else if( (op_code & 0xff) == 0xb0) {
		
			/* popl instruction */
			dest = (memory[pc + 1] & 0xf0) >> 4;

			if( (dest == 4) || (dest == 5)) {
				programStatus = INS;
				break;
			}

		} else {
			
			/* invalid instruction encountered */
			programStatus = INS;
		}




	}
}

/*
 * Register --> Register operation
*/
void rrmovl(int regA, int regB)
{
	steps++;
	reg[regB] = reg[regA];
	pc += 2;
}

/*
 * Immediate --> Register operation
*/
void irmovl(int val, int regA)
{
	steps++;
	reg[regA] = val;
printf("irmovl moveing value %x into reg[%d]\n reg[%d] now equals %08x\n", val, regA, regA, reg[regA]);
	pc += 5;
}

/*
 * Memory --> Register operation
*/
void mrmovl(int regA, int regB, unsigned int dest)
{
	steps++;
	reg[regB] = memory[dest + reg[regA]];
	pc += 5;
}

/*
 * Register --> Memory operation
*/
void rmmovl(int regA, int regB, unsigned int dest)
{
	steps++;
	memory[dest + reg[regB]] = reg[regA];
	pc += 5;
}

void set_flags(int function, unsigned int val1,
	 unsigned int val2, unsigned int result)
{

	/* Function codes for arithmetic operations for setting the
	conditional flags of the emulator 
	----------------------------------
	0 - add
	1 - subtract
	2 - and
	3 - xor
	*/

	flags[0] = 0;
	flags[1] = 0;
	flags[2] = 0;
	
	if(result == 0x0) {
	
		flags[0] = 1;
		flags[1] = 0;
		flags[2] = 0;

	} else if(result & 0x80000000) {

		flags[0] = 0;
		flags[1] = 1;
		flags[2] = 0;
	
	} else {

		if(function == 0) {

			if( ((val1 & 0x80000000) == 0x80000000)
				&& ((val2 & 0x80000000) == 0x80000000)
				&& ( ((val1 + val2) & 0x80000000)
				 != 0x80000000)) {
				
				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;

			} else if( ((val1 & 0x80000000) != 0x80000000)
				&& ((val2 & 0x80000000) != 0x80000000)
				&& ( ((val1 + val2) & 0x80000000)
				 == 0x80000000)) {

				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;
			}
	
		} else if(function == 1) {

			if( ((val1 & 0x80000000) == 0x80000000)
				&& ((val2 & 0x80000000) == 0x80000000)
				&& ( ((val1 - val2) & 0x80000000)
				 != 0x80000000)) {
				
				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;

			} else if( ((val1 & 0x80000000) != 0x80000000)
				&& ((val2 & 0x80000000) != 0x80000000)
				&& ( ((val1 - val2) & 0x80000000)
				 == 0x80000000)) {

				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;
			}

		} else if(function == 2) {
	
			if( ((val1 & 0x80000000) == 0x80000000)
				&& ((val2 & 0x80000000) == 0x80000000)
				&& ( ((val1 & val2) & 0x80000000)
				 != 0x80000000)) {
				
				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;

			} else if( ((val1 & 0x80000000) != 0x80000000)
				&& ((val2 & 0x80000000) != 0x80000000)
				&& ( ((val1 & val2) & 0x80000000)
				 == 0x80000000)) {

				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;
			}
		} else if(function == 3) {
		
			if( ((val1 & 0x80000000) == 0x80000000)
				&& ((val2 & 0x80000000) == 0x80000000)
				&& ( ((val1 ^ val2) & 0x80000000)
				 != 0x80000000)) {
				
				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;

			} else if( ((val1 & 0x80000000) != 0x80000000)
				&& ((val2 & 0x80000000) != 0x80000000)
				&& ( ((val1 ^ val2) & 0x80000000)
				 == 0x80000000)) {

				flags[0] = 0;
				flags[1] = 0;
				flags[2] = 1;
			}
		}
	}
}
/*
 * Arithmetic and Logical operations
*/
void opl(int function, int regA, int regB)
{
	unsigned int result;
	steps++;

	if(function == 0) {

		result = reg[regA] + reg[regB];
	} else if(function == 1) {
	
		result = reg[regA] - reg[regB];
	} else if(function == 2) {

		result = reg[regA] & reg[regB];
	} else if(function ==3) {

		result = reg[regA] ^ reg[regB];
	}
printf("values are %x and %x\n", reg[regA], reg[regB]);
printf("result: %x\n", result);
	set_flags(function, reg[regA], reg[regB], result);
	pc += 2;
}

/*
 * Jump operation
*/
void jump(int function, unsigned int dest)
{
	steps++;
}

/*
 * Conditional move operations
*/
void cmov(int function, int regA, int regB)
{
	steps++;
}

/*
 * Call operation
*/
void call(unsigned int dest)
{
	steps++;
}

/*
 * Return operation
*/
void ret()
{
	steps++;
}

/*
 * Pushes an item onto the stack
*/
void pushl(int regA)
{
	steps++;

	if( reg[4] - 4 <= stack_limit) {
		printf("Error: Stack Overflow detected.\n");
		programStatus = HLT;
		return;
	}

	reg[4] -= 4;
	memory[reg[4]] = reg[regA];
	pc += 2;
}

/*
 * Pops an item from the stack
*/
void popl(int regA)
{
	steps++;
	reg[regA] = memory[reg[4]];
	reg[4] += 4;
	pc += 2;
}

/*
 * Prints out a formatted table displaying various information about the
 * previous run of the emulator.
*/
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

		if(i == 4 || i == 5) {

			if(reg[i] != 0xffffffff) {
				
				flag = 1;
			}
		} else {
		
			if(reg[i] != 0x00000000) {
				
				flag = 1;
			}
		}
	}
	
	if(flag == 0) {
		printf("None\n");
	}
	
}
