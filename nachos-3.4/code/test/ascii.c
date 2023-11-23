#include "syscall.h"

int main() {
    unsigned char c = (unsigned char)(0);
    PrintString("ASCII TABLE\n");
    while (c <= (unsigned char)(255)) {
        PrintInt((int)c);
        PrintString(": ");
        PrintChar(c);

        PrintChar('\n');

        if (c == (unsigned char)255) {
            break;
        }

        c++;
    }

    Halt();
}
