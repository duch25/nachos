#include "syscall.h"

int main() {
    char c;
    PrintString("Enter character: ");
    c = ReadChar();
    PrintString("You Entered: ");
    PrintChar(c);
    PrintString("\n");
    Halt();
}
