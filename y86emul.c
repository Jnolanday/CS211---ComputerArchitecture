#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "y86emul.h"

char * strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}

char * substr(const char *inpStr, char *outStr, size_t startPos, size_t strLen) {
	strncpy(outStr, inpStr + startPos, strLen);
	outStr[strLen] = '\0';
	return outStr;
}

char * openFile(const char * filename) { /*Opens the file and checks whether it exists or is not empty*/
	char * file = 0;
	long length;
	FILE * fp = fopen (filename, "rb");
	if (fp) {
		fseek (fp, 0, SEEK_END);
		length = ftell (fp);
		fseek (fp,  0, SEEK_SET);
		file = malloc (length);
		if (file) {
			fread (file, 1, length, fp);
		}
		fclose (fp);
	} else {
		fprintf(stderr,"ERROR: File does not exist.\n");
                exit(EXIT_FAILURE);
	}
	if (file) {
		return file;
	} else {
		fprintf(stderr,"ERROR: File is empty. \n");
                exit(EXIT_FAILURE);
		return "";
	}
}

int FDEMWU(unsigned char * mem, y86arch * y86arch) {/*Stands for Fetch, Decode, Execute, Memory, Write back, and Update PC from the loaded memory*/
	int DEBUG = 0;
	unsigned char rA;
	unsigned char rB;

	char input;
	int iin;

	int  val;
	unsigned long  dest;
	unsigned int disp;

	y86arch->cont = true;

	while(y86arch->cont){				/*Switch statements that reads through the loaded memory,
					 		 * and performs FDEMWU. */
		switch (mem[y86arch->PC]) {
			case 0x00: /* nop */
				printf("nop\n");
				y86arch->PC++;
				break;
			case 0x10: /* halt */
				printf(HLT);
				printf(AOK);
				y86arch->cont = false;
				return 0;
				break;
			case 0x20: /* rrmovl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG) printf("rrmovl\t%u\t%u\n",rA,rB);
				y86arch->reg[rB] = y86arch->reg[rA];

				y86arch->PC += 2;

				break;
			case 0x30: /* irmovl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				val =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);
				if(DEBUG) printf("rrmovl\t%u\t%u\n",rA,rB);
				y86arch->reg[rB] = val;

				y86arch->PC += 6;

				break;
			case 0x40: /* rmmovl */
                                if(DEBUG)printf("rmmovl\n");
				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG) printf("rmmovl\t%d\t%d\n",rA,rB);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);

				mem[y86arch->reg[rB] + disp + 0] = (y86arch->reg[rA] & 0x000000ff);
				mem[y86arch->reg[rB] + disp + 1] = (y86arch->reg[rA] & 0x0000ff00) >> 8;
				mem[y86arch->reg[rB] + disp + 2] = (y86arch->reg[rA] & 0x00ff0000) >> 16;
				mem[y86arch->reg[rB] + disp + 3] = (y86arch->reg[rA] & 0xff000000) >> 24;

				y86arch->PC += 6;

				break;
			case 0x50: /* mrmovl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);

				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);
				if(DEBUG)printf("mrmovl\t%d\t%d\n",rA,rB);
				y86arch->reg[rA] =  (mem[y86arch->reg[rB] + disp + 0])
				                 +  (mem[y86arch->reg[rB] + disp + 1]  <<  8)
			                         +  (mem[y86arch->reg[rB] + disp + 2]  << 16)
				               	 +  (mem[y86arch->reg[rB] + disp + 3]  << 24);

				y86arch->PC += 6;

				break;
			case 0xe0: /* movsbl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG) printf("movsbl\t%d\t%d\n",rA,rB);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);

				val =  (mem[y86arch->reg[rB] + disp + 0])
             			    +  (mem[y86arch->reg[rB] + disp + 1]  <<  8)
                  		    +  (mem[y86arch->reg[rB] + disp + 2]  << 16)
                		    +  (mem[y86arch->reg[rB] + disp + 3]  << 24);
				y86arch->reg[rA] = val << 24;
				y86arch->PC += 6;
				break;

			case 0x60: /* addl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("addl\t%d\t%d\n",rA,rB);
				y86arch->OF = 0;
				y86arch->SF = 0;
				y86arch->ZF     = 0;

				val = y86arch->reg[rA] + y86arch->reg[rB];

				if ((y86arch->reg[rA] > 0 && y86arch->reg[rB] > 0 && val < 0) ||
			            (y86arch->reg[rA] < 0 && y86arch->reg[rB] < 0 && val > 0)) {
					y86arch->OF = 1;
				}

				if (val == 0) {
					y86arch->ZF = 1;
				}

				if (val < 0) {
					y86arch->SF = 1;
				}

				y86arch->reg[rB] = val;
				y86arch->PC += 2;

				break;
			case 0x61: /* subl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("subl\t%d\t%d\n",rA,rB);
				y86arch->OF = 0;
				y86arch->SF = 0;
				y86arch->ZF     = 0;

				val = y86arch->reg[rB] - y86arch->reg[rA];

				if ((y86arch->reg[rA] < 0 && y86arch->reg[rB] > 0 && val < 0) ||
			            (y86arch->reg[rA] > 0 && y86arch->reg[rB] < 0 && val > 0))
					y86arch->OF = 1;


				if (val == 0) {
					y86arch->ZF = 1;
				}

				if (val < 0) {
					y86arch->SF = 1;
				}

				y86arch->reg[rB] = val;
				y86arch->PC += 2;

				break;
			case 0x64: /* imull */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("imull\t%d\t%d\n",rA,rB);
				y86arch->OF = 0;
				y86arch->SF = 0;
				y86arch->ZF = 0;

				val = y86arch->reg[rA] * y86arch->reg[rB];

				if ((y86arch->reg[rA] > 0 && y86arch->reg[rB] > 0 && val < 0) ||
				    (y86arch->reg[rA] < 0 && y86arch->reg[rB] < 0 && val < 0) ||
				    (y86arch->reg[rA] < 0 && y86arch->reg[rB] > 0 && val > 0) ||
				    (y86arch->reg[rA] > 0 && y86arch->reg[rB] < 0 && val > 0)) {
					y86arch->OF = 1;
				}

				if (val == 0) {
					y86arch->ZF = 1;
				}

				if (val < 0) {
					y86arch->SF = 1;
				}

				y86arch->reg[rB] = y86arch->reg[rA] * y86arch->reg[rB];
				y86arch->PC += 2;

				break;
			case 0x62: /* andl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)	printf("andl\t%d\t%d\n",rA,rB);
				y86arch->SF = 0;
				y86arch->ZF     = 0;

				val = rA & rB;

				if (val == 0) {
					y86arch->ZF = 1;
				}

				if (val < 0) {
					y86arch->SF = 1;
				}

				y86arch->PC += 2;


				break;
			case 0x63: /* xorl */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG) printf("xorl\t%d\t%d\n",rA,rB);
				y86arch->SF = 0;
				y86arch->ZF     = 0;

				val = rA ^ rB;

				if (val == 0) {
					y86arch->ZF = 1;
				}

				if (val < 0) {
					y86arch->SF = 1;
				}


				y86arch->PC += 2;

				break;
			case 0x65: /* cmpl */

              			rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
                		rB = (mem[y86arch->PC + 1] & 0x0f);
						if(DEBUG) printf("cmpl\t%d\t%d\n",rA,rB);
                		y86arch->SF = 0;
                		y86arch->ZF = 0;
                		y86arch->OF = 0;

                		if(y86arch->reg[rA]==y86arch->reg[rB]){
                        		y86arch->ZF = 1;
                		}
                		if(y86arch->reg[rA]<y86arch->reg[rB]){
                        		y86arch->SF = 1;
                		}

                		y86arch->PC += 2;
                		break;

			case 0x70: /* jmp */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jmp\t%u\n",dest);
				y86arch->PC = dest;

				break;
			case 0x71: /* jle */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jle\t%u\n",dest);
				if (y86arch->SF ^ y86arch->OF || y86arch->ZF)
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x72: /* jl */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jl\t%u\n",dest);
				if (y86arch->SF ^ y86arch->OF)
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x73: /* je */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("je\t%u\n",dest);
				if (y86arch->ZF)
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x74: /* jne */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jne\t%u\n",dest);
				if (!y86arch->ZF)
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x75: /* jge */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jmp\t%u\n",dest);
				if (!(y86arch->SF ^ y86arch->OF))
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x76: /* jg */

				dest =    mem[y86arch->PC + 1]
				      + (mem[y86arch->PC + 2] << 8)
				      + (mem[y86arch->PC + 3] << 16)
				      + (mem[y86arch->PC + 4] << 24);
				if(DEBUG)printf("jg\t%u\n",dest);
				if (!(y86arch->SF ^ y86arch->OF) && !y86arch->ZF)
					y86arch->PC = dest;
				else
					y86arch->PC += 5;

				break;
			case 0x80: /* call */

				dest =    mem[y86arch->PC + 1]
				       + (mem[y86arch->PC + 2] << 8)
				       + (mem[y86arch->PC + 3] << 16)
				       + (mem[y86arch->PC + 4] << 24);

				if(DEBUG)printf("call\t%u\n",dest);
				y86arch->reg[ESP] -= 4;
				iin = y86arch->PC + 5;
				val = 0;

				mem[y86arch->reg[ESP] + 0] = (iin & 0x000000ff);
				mem[y86arch->reg[ESP] + 1] = (iin & 0x0000ff00) >> 8;
				mem[y86arch->reg[ESP] + 2] = (iin & 0x00ff0000) >> 16;
				mem[y86arch->reg[ESP] + 3] = (iin & 0xff000000) >> 24;

				y86arch->PC = dest;

				break;
			case 0x90: /* ret */
                                printf("ret\n");
				y86arch->PC = (mem[y86arch->reg[ESP] + 0])
				                     + (mem[y86arch->reg[ESP] + 1] << 8)
				                     + (mem[y86arch->reg[ESP] + 2] << 16)
				                     + (mem[y86arch->reg[ESP] + 3] << 24);

				y86arch->reg[ESP] += 4;

				break;
			case 0xa0: /* pushl */
				if(DEBUG)      printf("pushl\n");
				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);

				y86arch->reg[ESP] -= 4;

				mem[y86arch->reg[ESP] + 0] = (y86arch->reg[rA]);
				mem[y86arch->reg[ESP] + 1] = (y86arch->reg[rA] & 0x0000ff00) >> 8;
				mem[y86arch->reg[ESP] + 2] = (y86arch->reg[rA] & 0x00ff0000) >> 16;
				mem[y86arch->reg[ESP] + 3] = (y86arch->reg[rA] & 0xff000000) >> 24;

				y86arch->PC += 2;
				break;
			case 0xb0: /* popl */
                                printf("popl\n");
				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("popl\t%d\t%d\n",rA,rB);
				y86arch->reg[rA] = (mem[y86arch->reg[ESP] + 0])
				                    + (mem[y86arch->reg[ESP] + 1] << 8)
				                    + (mem[y86arch->reg[ESP] + 2] << 16)
				                    + (mem[y86arch->reg[ESP] + 3] << 24);

				y86arch->reg[ESP] += 4;
				y86arch->PC += 2;
				break;
			case 0xc0: /* readb */
                                printf("readb\n");
				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("readb\t%d\t%d\n",rA,rB);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);

				if (EOF == scanf("%c", &input)) {
					y86arch->ZF = 1;
				} else y86arch->ZF = 0;

				mem[y86arch->reg[rA] + disp] = input;

				y86arch->PC += 6;
				break;
			case 0xc1: /* readw */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("readw\t%d\t%d\n",rA,rB);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);

				if (EOF == scanf("%i", &iin)) {
					y86arch->ZF = 1;
				} else y86arch->ZF = 0;

				mem[y86arch->reg[rA] + disp + 0] =  iin & 0x000000ff;
				mem[y86arch->reg[rA] + disp + 1] = (iin & 0x0000ff00) >> 8;
				mem[y86arch->reg[rA] + disp + 2] = (iin & 0x00ff0000) >> 16;
				mem[y86arch->reg[rA] + disp + 3] = (iin & 0xff000000) >> 24;

				y86arch->PC += 6;

				break;
			case 0xd0: /* writeb */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);
				if(DEBUG)printf("writeb\t%d\t%d\n",rA,rB);
				printf("%c",mem[y86arch->reg[rA] + disp]);

				y86arch->PC += 6;

				break;
			case 0xd1: /* writew */

				rA = (mem[y86arch->PC + 1] & 0xf0) >> 4;
				rB = (mem[y86arch->PC + 1] & 0x0f);
				if(DEBUG)printf("writew\t%d\t%d\n",rA,rB);
				disp =    mem[y86arch->PC + 2]
				      + (mem[y86arch->PC + 3] << 8)
				      + (mem[y86arch->PC + 4] << 16)
				      + (mem[y86arch->PC + 5] << 24);

				val =  (mem[y86arch->reg[rB] + disp + 0])
				     + (mem[y86arch->reg[rB] + disp + 1] << 8)
			             + (mem[y86arch->reg[rB] + disp + 2] << 16)
				     + (mem[y86arch->reg[rB] + disp + 3] << 24);

				printf("%i",val);

				y86arch->PC += 6;
				break;
			default:
				fprintf(stderr, INS); /* If an unknown instruction is found */
			return 1;

				break;
		}
		val = 0;
		dest = 0;
		disp = 0;
		rA = 0;
		rB = 0;
	}

	fprintf(stderr, ADR);
	return 1;
}

int loadMemory(char * program) { /*Loads "memory" of machine instructions and values from y86 input file */
	char * dup = strdup(program);
	char * tok = strtok(dup, "\t\n");

	int size;

	int i;
	int j;

	char * directive; /* Stores directives */
	char * hexAddr; /*Stores hex addresses */
	char * argument; /*Stores argument values */

	int longarg;

	char * subarg;

	y86arch * y86arch; /*The y86 struct that represents the y86 architecture: registers,conditional codes, and program counter */
	unsigned char * mem; /* Array that represents memory which holds machine instructions and data */

	while (tok) {
		if (strcmp(tok, ".size") == 0) {

			tok = strtok(NULL, "\t\n");
			size = strtoll(tok,NULL,16);
		} else {
			tok = strtok(NULL, "\t\n");
		}
	}

	free(dup);

	/*Process of loading instructions and data into memory(the memory array)*/
	y86arch = malloc(sizeof *y86arch);

	y86arch->PC = -1;
	y86arch->size = size;

	mem = malloc((size + 1) * sizeof(unsigned char*));

	for (i = 0; i < size; i++) {
		mem[i] = 0;
	}

	for (i = 0; i < 9; i++) {
		y86arch->reg[i] = 0;
	}

	dup = strdup(program );
	tok = strtok(dup, "\t\n");

	i = 0;

	while (tok) {
		if (strcmp(tok, ".size") == 0) {/*skip .size directive and argument */
			tok = strtok(NULL, "\t\n");
			tok = strtok(NULL, "\t\n");
		} else if (strcmp(tok, ".string") == 0 || /*checks for valid command */
			   strcmp(tok, ".long") == 0 ||
		 	   strcmp(tok, ".bss") == 0 ||
			   strcmp(tok, ".byte") == 0 ||
			   strcmp(tok, ".text") == 0) {
			directive = strdup(tok); /* get directive */
			tok = strtok(NULL, "\n\t"); /* move past directive */

			hexAddr = strdup(tok);
			tok = strtok(NULL, "\n\t");

			argument = strdup(tok); /*Get argument after hex address for particular directive*/
			tok = strtok(NULL, "\n\t");

			if (strcmp(directive, ".string") == 0) {	/*for .string directive*/
				i = strtoll(hexAddr,NULL,16);
				j = 1;

				while (j <= strlen(argument)) {
					mem[i] = (unsigned char) argument[j];

					i++;
					j++;
				}
			} else if ( strcmp(directive, ".text") == 0) { /*for .text directive*/

				i = strtoll(hexAddr,NULL,16);
				j = 0;

				if (y86arch->PC == -1) {
					y86arch->PC = i;
				} else if (y86arch->PC != -1) {
					fprintf(stderr, INS);
					return 1;
				}

				while (j < strlen(argument)) {
					/* in substrings of length 2*/
					subarg = substr(argument, subarg, j, 2);
					/* convert it to decimal */
					mem[i] = (unsigned char) strtoll(subarg,NULL,16);

					i++;
					j = j + 2;
				}

			} else if (strcmp(directive, ".long") == 0) { /*for .long directive */
				longarg = atoi(argument);
				j = strtoll(hexAddr,NULL,16);
				*(unsigned int*)(mem+j) = longarg;
			} else if (strcmp(directive, ".byte") == 0) { /* for .byte directive */
				mem[strtoll(hexAddr,NULL,16)] = (unsigned char)strtoll(argument,NULL,16);
			}

			free(hexAddr);
                	free(argument);
			free(directive);
			hexAddr = 0;
		} else {
			fprintf(stderr, INS); /*Error if an unknown directive is found */
			return 1;
		}
	}

	free(dup);
	free(tok);

	return FDEMWU(mem, y86arch);
}

int main(int argc, const char * argv[]) {
	char * ptr = strchr(argv[1],'.');
	char * fileType = malloc(3*sizeof(unsigned char));
	if(ptr){
		int index = ptr - argv[1];
		substr(argv[1],fileType,index,4);
		if(strcmp(fileType,".y86")!=0){
			printf("Unsupported file type. Expect .y86 file type.\n");		/*Handling for unsupported
																			*file type input files*/
			return 1;
		}
	}

    if (strcmp(argv[1],"-h")==0) {           /*implementation of Usage message for help flag*/
            printf("Usage: y86emul  <y86 input file>\n");
    }else{
            return loadMemory(openFile(argv[1]));
    }
}
