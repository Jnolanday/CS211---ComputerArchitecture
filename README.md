# CS211 CS211 Computer Architecture Summer 2017
### Final Project: y86 Assembly Emulator
By Jarisha Olanday
## Description
### Usage
Compiling and linking necessary files is made simple using the "make" command in terminal.

Run the program with the following statement:

  ``` ./y86emul [-h] <y86 input file>```
 
 The help flag [-h] with output a usage message.
 
 ### Summary
 An emulator for Y86 Assembly instructions as represented in ASCII in the input file.
 
 Example input file (program for a factorial calculator):
 ```.size	1800
.string	04	"Enter an integer>>"
.long	20	0
.long	24	0
.byte	28	0a
.text	100	30f4001800002045804a0100001000a05f204530f01000000061044075fcffffff30f001000000627773450100005035fcffffff61032037800f0100005035fcffffff64302054b05f90a05f204530f30400000030f00100000030f212000000d03f0000000060036102746001000030f120000000c11f0000000073ae010000507100000000800f01000030f124000000400100000000d11f0000000030f128000000d01f00000000704e0100002054b05f90
```
The y86emul program consists of 3 main functions: `openFile`, `loadMemory`, and `FDEMWU` (acronym for the stages for the Y86 instruction compilation and execution).

The first fuction on the list, `openFile`, opens the file and checks whether it exists or if it's empty. The appropriate error will output if either is the case.

The next function `loadMemory`, reads through the file and parses the directives and their arguments. It first finds the .size directive and reads the argument size. The size value is the size of memory allocated for the *mem* array used in the function to represent memory which will hold the machine instructions and data. The indexes of the array correspond to addresses. The value from the .size directive is also the size of memory allocated for the *y86arch* struct which is used to represent parts of the y86 architechture. The fields of the *y86arch* struct includes reg (an array that represents the 8 registers where the indexes of the array correspond to a particular register), PC (program counter), size, and the condition codes for overflow, negative, and zero flags. After the .size directive, the `loadMemory` function reads through the rest of the input file directives. For each directive, it immediately places the data argument into the memory array at the index of the hex address. For the .text directive in particular, the PC is also set to the hex address. If any unknown directive is found, it throws an error. If no .size or .text directive is found, it throws an error. Otherwise, the function returns the value of the function `FDEMWU` with the memory array and y86arch struct as parameters.

The final function `FDEMWU` reads through the instructions from the .text directive argument stored in the memory array created from the `loadMemory` and performs the stages of Fetch, Decode, Execute, Memory, Write Back, and Update PC for each instruction. It reads from the mem array at index y86arch->PC value which stores the hex address value specified from the .text directive. It then loops through a switch statement for each y86 instruction in which each instruction has it's own FDEMWU stages. An error is thrown if an unidentifiable instruction code or invalid address is found. For debug purposes, I have a boolean value DEBUG that, when set to true (set to 1), the program prints the instruction names and the registers or destination values at those instructions. I used that to check if the register/destination values were read and calculated correctly.
