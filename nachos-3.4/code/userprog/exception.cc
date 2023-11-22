// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

void ReadInt()
{
    char *buffer;
    buffer = new char[256];
    int len, number;
    bool validNumber;
    len = synchConsole->Read(buffer, 256);

    number = 0;
    validNumber = true;

    if (len == 11 && buffer[0] == '-' && buffer[1] != '0')
    {
        char *minInt = (char *)"-2147483648";
        bool isSmaller = false;
        for (int i = 1; i < len; ++i)
        {
            if (buffer[i] >= '0' && buffer[i] <= '9')
            {
                int digit = buffer[i] - '0';
                if (!isSmaller && buffer[i] < minInt[i])
                {
                    isSmaller = true;
                    number = number * 10 + digit;
                }
                else if (isSmaller || (!isSmaller && buffer[i] <= minInt[i]))
                {
                    number = number * 10 + digit;
                }
                else
                {
                    validNumber = false;
                    break;
                }
            }
            else
            {
                validNumber = false;
                break;
            }
        }
        if (validNumber)
        {
            number = -number;
        }
    }
    else if (len < 11)
    {
        if (len == 10 && buffer[0] != '-' && buffer[1] != '0')
        {
            char *maxInt = (char *)"2147483647";
            bool isSmaller = false;
            for (int i = 0; i < len; ++i)
            {
                if (buffer[i] >= '0' && buffer[i] <= '9')
                {
                    int digit = buffer[i] - '0';
                    if (!isSmaller && buffer[i] < maxInt[i])
                    {
                        isSmaller = true;
                        number = number * 10 + digit;
                    }
                    else if (isSmaller || (!isSmaller && buffer[i] <= maxInt[i]))
                    {
                        number = number * 10 + digit;
                    }
                    else
                    {
                        validNumber = false;
                        break;
                    }
                }
                else
                {
                    validNumber = false;
                    break;
                }
            }
        }
        else
        {
            int l = 0, mul = 1;
            if (buffer[0] == '-')
            {
                l = 1, mul = -1;
            }
            bool leadingZero = true;
            for (int i = l; i < len; ++i)
            {
                if (buffer[i] >= '0' && buffer[i] <= '9')
                {
                    if (leadingZero && buffer[i] == '0')
                    {
                        validNumber = false;
                        break;
                    }
                    else if (leadingZero && buffer[i] > '0')
                    {
                        leadingZero = false;
                    }
                    int digit = buffer[i] - '0';
                    number = number * 10 + digit;
                }
                else
                {
                    validNumber = false;
                    break;
                }
            }
            number = number * mul;
        }
    }

    if (!validNumber)
    {
        number = 0;
    }
    machine->WriteRegister(2, number);
    delete buffer;
    // printf("%d\n", number);
    return IncreasePC();
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException: // Everything ok!
        return;
    case PageFaultException:
        DEBUG('a', "No valid translation found.\n");
        printf("No valid translation found.\n");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "Write attempted to page marked read-only.\n");
        printf("Write attempted to page marked read-only.\n");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "Translation resulted in an invalid physical address.\n");
        printf("Translation resulted in an invalid physical address.\n");
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "Unaligned reference or one that was beyond the end of the address space.\n");
        printf("Unaligned reference or one that was beyond the end of the address space.\n");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "Integer overflow in add or sub.\n");
        printf("Integer overflow in add or sub.\n");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "Unimplemented or reserved instr.\n");
        printf("Unimplemented or reserved instr.\n");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "Number exception types.\n");
        printf("Number exception types.\n");
        interrupt->Halt();
        break;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "Shutdown,initiated by user program.\n");
            printf("Shutdown,initiated by user program.\n");
            interrupt->Halt();
            return;
        case SC_ReadInt:
            return ReadInt();
        default:
            printf("Unexpected user mode exception %d %d\n", which, type);
            ASSERT(FALSE);
        }
    default:
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
