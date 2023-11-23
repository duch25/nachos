#include "syscall.h"


int main() {
	int n;
	n = ReadInt();
	PrintInt(n);
	Halt();
}
