#include "syscall.h"

int main() {

    unsigned char c;
    int n;
    n = 0;
    c = (unsigned char)(32);

    PrintString("ASCII TABLE\n");
    while (c <= (unsigned char)(127)) {
	n++;
        PrintInt((int)c);
        PrintString(": ");
        PrintChar(c);

        PrintChar('\t');
	if(n==6) {
		PrintChar('\n');
                n = 0;
	}

        c++;
    }
    PrintChar('\n');

    Halt();
}
