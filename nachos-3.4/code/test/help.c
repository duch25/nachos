#include "syscall.h"

int main()
{
    PrintString("=== GROUP INFORMATION\n");
    PrintString("1. 21120412 - Gia-Bach Tran\n");
    PrintString("2. 21120450 - The-Hien Truong\n");
    PrintString("3. 21120454 - Duc-Hoai Nguyen\n");
    PrintString("4. 21120468 - Gia-Huy Nguyen\n");

    PrintString("\n");
    PrintString("=== PROGRAM INSTRUCTIONS\n");
    PrintString("1. ASCII: at code directory, run ./userprog/nachos -rs 1023 -x ./test/ascii to print the ASCII table\n");
    PrintString("2. Sort: at code directory, run ./userprog/nachos -rs 1023 -x ./test/sort to start the sort program\n");

    PrintString("\n");

    Halt();
}