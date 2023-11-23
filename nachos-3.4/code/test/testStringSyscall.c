#include "syscall.h"

int main()
{
    char str[1020];

    PrintString("===== TEST SYSTEM CALL READSTRING, PRINTSTRING ===== \n");

    PrintString("Enter a any string: ");
    ReadString(str, 1020);

    PrintString("You entered: ");
    PrintString(str);
    PrintString("\n");
    
    Halt();
}