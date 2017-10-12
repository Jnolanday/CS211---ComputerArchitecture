#define AOK "Everything is fine. No errors detected during execution\n"
#define HLT "Halt instruction encountered.\n"
#define ADR "Invalid address encountered.\n"
#define INS "Invalid instruction encountered.\n"

#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

typedef struct y86architecture {/*Struct that represents parts of the y86 architecture*/
	int reg[9];/*The eight registers */
	int PC;/*program counter*/
	int size;

	bool  cont;
	int  OF;/*Condition code for overflow flag*/
	int  ZF;/*Condition code for zero flag*/
	int  SF;/*Condition code for negative flag*/
} y86arch;
