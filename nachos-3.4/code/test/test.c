#include "syscall.h"


int main() {
	int n;
	PrintString("Enter integer: ");
	n = ReadInt();
	PrintString("Output: ");
	PrintInt(n);
	PrintString("\n");	
	Halt();
}
