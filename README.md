# vPC-v4

VM works on the concept of functions and independant memories for each function.
VM loads function templates at the start and pushes a copy of the main function to the function stack.
During operation it works on the top function in the stack. After the instruction coutner has exceeded the number
of instructions in a function, the function is removed from the stack. If no more functions are left, the CPU halts.

A function memory consists of vectors which contain unsigned ints and VM can on demand 
allocate more vectors or expand already existing vectors.

Global memory also exists which can be accessed from anywhere.

The VM currenty has 8 general purpose registers, halt flag and 57 instructions.

It has basic terminal IO, more advanced IO with files etc. will be added later on.

An assembler for the VM is available in assembler/.
