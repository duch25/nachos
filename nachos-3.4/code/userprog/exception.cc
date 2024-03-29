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

// MAX_SIZE cho viec xu ly cac syscall lien quan den chuoi
#define MAX_SIZE 1000
#define MAX_LENGTH_FILENAME 32
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

// Tang thanh ghi PC
void increasePC()
{
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

// Ham xu ly system call nhap so nguyen
int readInt()
{
    // Nhap chuoi tu console
    char *buffer;
    buffer = new char[256];
    int len, number;
    bool validNumber;
    len = synchConsole->Read(buffer, 256);

    number = 0;
    validNumber = true;

    // Case 1: INT_MIN -> -2147483648
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
            else // Khong phai la chu so
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
        if (len == 10 && buffer[0] != '-' && buffer[1] != '0') // Case 2: INT_MAX -> 2147483647
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
                else // Khong phai la chu so
                {
                    validNumber = false;
                    break;
                }
            }
        }
        else // Case 3: Tat ca cac truong hop con lai
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
                else // Khong phai la chu so
                {
                    validNumber = false;
                    break;
                }
            }
            number = number * mul;
        }
    }

    if (!validNumber) // Neu khong phai la so nguyen hop le -> 0
    {
        number = 0;
    }
    delete buffer;
    return number;
}

void ReadIntHandler()
{
    int n;
    n = readInt();
    machine->WriteRegister(2, n); // Luu gia tri so nguyen vao thanh ghi so 2
    return increasePC();
}

void printInt(char* buffer, int numberOfNum) {
    synchConsole->Write(buffer, numberOfNum);
}

void PrintIntHandler()
{
    // Chuc nang: In so nguyen len man hinh console
	int number = (int)machine->ReadRegister(4);
    char *buffer;

	// Qua trinh chuyen doi so thanh chuoi de in ra man hinh
    bool isNegative = false; // input la so duong
    int numberOfNum = 0;     // Bien de luu so luong chu so cua number
    int firstNumIndex = 0;
    
    // Truong hop so 0
    if (number == 0)
    {
        buffer = new char[1];
        buffer[0] = '0';
		numberOfNum = 1;
    }
	else {
		// Tao buffer chuoi de in ra man hinh
		buffer = new char[11]; 
		bool isMinInt = false;
		
		if (number < 0) {
			isNegative = true;
			
			if (number == -2147483648) {
				isMinInt = true;
				// printf("%d", number % 10);
				buffer[9] = (char)((number % 10) * -1 + 48);
				number /= 10;
			}
			
			number = number * -1; // Chuyen so am thanh so duong de tinh so chu so
		}
		
		int tmp_number = number; // Bien tam cho number
		while (tmp_number) {
			numberOfNum++;
			tmp_number /= 10;
		}
	

		for (int i = numberOfNum - 1; i >= 0; i--) {
			buffer[i] = (char)((number % 10) + 48);
			number /= 10;
		}
		

		// Truong hop la so am
		if (isNegative) {
			printInt("-", 1);
		}
		
		if (isMinInt) numberOfNum++;
		buffer[numberOfNum] = 0;
	}
    
	printInt(buffer, numberOfNum);
    
    delete buffer;
    return increasePC();
}
/*
// Cai dat ham readChar tu console
char readChar()
{
    char ch = synchConsole->getChar();
    return ch;
}
*/
/*Cai dat syscall ReadChar*/
/*
void ReadCharHandler()
{
    char c;
    c = readChar();
    machine->WriteRegister(2, c);
    return increasePC();
}

// Cai dat printChar
void printChar(char c)
{
    synchConsole->putChar(c);
}

void PrintCharHandler()
{
    // Doc ki tu thanh ghi 4 la tham so dau vao va gan cho c
    char c = (char)machine->ReadRegister(4);
    // In ki tu c
    printChar(c);
    // Tang thanh ghi PC
    return increasePC();
}

// ham copy vung nho tu user space vao system space
// Input:
//      User space address(int)
//      Limit of buffer(int)
// Output:
//      Buffer(char *)
*/
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;

    kernelBuf = new char[limit + 1]; // need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;

        if (oneChar == 0)
            break;
    }

    return kernelBuf;
}

// ham copy vung nho tu system space vao user space
// Input:
//      User space address (int)
//      Limit of buffer (int)
//      Buffer (char[])
// Output:
//      Number of bytes copied(int)

int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;

    if (len == 0)
        return len;

    int i = 0;
    int oneChar = 0;

    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);

    return i;
}
/*
// xu ly system call ReadString
void readString(char buffer[], int length)
{
    for (int i = 0; i < length - 1; ++i)
    {
        // doc tung ki tu vao buffer
        buffer[i] = readChar();

        // ki tu ket thuc (enter)
        if (buffer[i] == '\012')
        {
            buffer[i] = '\0';
            break;
        }
    }

    buffer[length - 1] = '\0';
}

// xu ly system call PrintString
void printString(char buffer[])
{
    char *temp = buffer;

    // in tung ki mot cho den khi gap ki tu ket thuc: 0
    while (*temp)
    {
        printChar(*temp);
        temp++;
    }
}

void ReadStringHandler()
{
    // lay gia tri cac tham so
    int virtualAddr = machine->ReadRegister(4);
    int length = machine->ReadRegister(5);

    // chuoi vuot qua gioi han chieu dai
    if (length < 0 || length > MAX_SIZE)
    {
        printString((char *)"Invalid string!\n");
        interrupt->Halt();
        return;
    }

    // copy vung nho tu userspace vao system space
    char *buffer = User2System(virtualAddr, length);

    // doc chuoi
    readString(buffer, length);

    // copy vung nho tu system space vao user space
    System2User(virtualAddr, length, buffer);

    delete[] buffer;
}

void PrintStringHandler()
{
    // lay gia tri cac tham so
    int virtualAddr = machine->ReadRegister(4);
    char *buffer = NULL;

    while (true)
    {
        // copy vung nho tu userspace vao system space theo limit size
        buffer = User2System(virtualAddr, MAX_SIZE);
        printString(buffer);

        // kiem tra ket thuc chuoi
        if (buffer[MAX_SIZE - 1] == 0)
        {
            delete[] buffer;
            break;
        }

        virtualAddr += MAX_SIZE;
        delete[] buffer;
    }
}
*/

void ReadCharHandler(){
	int maxBytes = 255;
	char* buffer = new char[255];
	int numBytes = synchConsole->Read(buffer, maxBytes);

	if(numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
	{
		printf("Chi duoc nhap duy nhat 1 ky tu!");
		DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
		machine->WriteRegister(2, 0);
	}
	else if(numBytes == 0) //Ky tu rong
	{
		printf("Ky tu rong!");
		DEBUG('a', "\nERROR: Ky tu rong!");
		machine->WriteRegister(2, 0);
	}
	else
	{
		//Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
		char c = buffer[0];
		machine->WriteRegister(2, c);
	}

	delete buffer;
	return;
}

void PrintCharHandler(){
	char c = (char)machine->ReadRegister(4);
	synchConsole->Write(&c, 1);
	increasePC();
	return;
}

void ReadStringHandler(){
	int virtAddr, length;
	char* buffer;
	virtAddr = machine->ReadRegister(4);
	length = machine->ReadRegister(5); 
	buffer = User2System(virtAddr, length); 
	synchConsole->Read(buffer, length); 
	System2User(virtAddr, length, buffer); 
	delete buffer; 
	increasePC();
	return;
}
void PrintStringHandler(){
	int virtAddr;
	char* buffer;
	virtAddr = machine->ReadRegister(4); 
	buffer = User2System(virtAddr, 255);
	int length = 0;
	while (buffer[length] != 0) length++;
	synchConsole->Write(buffer, length + 1);
	delete buffer; 
	increasePC();
	return;	
}



void CreateFileHandler() {
	
	int virtualAddr = machine->ReadRegister(4);
	char* fileName = User2System(virtualAddr, 33);
	if (fileName == NULL || strlen(fileName) == 0) {
		machine->WriteRegister(2, -1); 
		return;
	}
	if (!fileSystem->Create(fileName, 0)) {
		machine->WriteRegister(2, -1);
		delete fileName;
		return;
	}
	machine->WriteRegister(2, 0);
	delete fileName;
}

void OpenHandler() {
	int virtualAddr = machine->ReadRegister(4); 
	int type = machine->ReadRegister(5); 
	
	char* fileName = User2System(virtualAddr, 32);
	
	int freeSlot = fileSystem->FindFreeSlot();
	if (freeSlot != -1 && type >= 0) {
		if (type <= 1) {
			fileSystem->openf[freeSlot] = fileSystem->Open(fileName, type);
			if (fileSystem->openf[freeSlot] != NULL) {
				machine->WriteRegister(2, freeSlot); 
			} else {
				machine->WriteRegister(2, -1); 
			}
		} else if (type <= 3) {
			machine->WriteRegister(2, type - 2); 
		} else {
			printf("Invalid type!!!\n");
			machine->WriteRegister(2, -1);
		}
	} else machine->WriteRegister(2, -1); 
	delete fileName;
}

void CloseHandler() {
	int fileID = machine->ReadRegister(4);
	if (fileID >= 0 && fileID <= 9 && fileSystem->openf[fileID]) {
		delete fileSystem->openf[fileID]; 
		fileSystem->openf[fileID] = NULL; 
		machine->WriteRegister(2, 0);
		return;
	}
	machine->WriteRegister(2, -1);
}

void ReadHandler() {
	int virtualAddr = machine->ReadRegister(4); 
	int charcount = machine->ReadRegister(5); 
	int fileID = machine->ReadRegister(6);
	int oldPos;
	char* buffer;
	if (fileID < 0 || fileID > 9 || fileID == 1 || fileSystem->openf[fileID] == NULL) {
		printf("Khong the doc!!!\n");
		machine->WriteRegister(2, -1);
	} else {
		oldPos = fileSystem->openf[fileID]->getSeekPosition(); 
		buffer = User2System(virtualAddr, charcount);
		if (fileID == 0) { // stdin
			int numBytes = synchConsole->Read(buffer, charcount); 
			machine->WriteRegister(2, numBytes);
			System2User(virtualAddr, numBytes, buffer);
		} else if (fileSystem->openf[fileID]->Read(buffer, charcount)) {
			int newPos = fileSystem->openf[fileID]->getSeekPosition();
			System2User(virtualAddr, newPos - oldPos, buffer); 
			machine->WriteRegister(2, newPos - oldPos);
		} else {
			machine->WriteRegister(2, -2);
		}
		delete buffer;
	}
}

void WriteHandler() {
	int virtualAddr = machine->ReadRegister(4); 
	int charcount = machine->ReadRegister(5); 
	int fileID = machine->ReadRegister(6);
	int oldPos;
	char* buffer;
	if (fileID < 0 || fileID > 9 || fileID == 0 || fileSystem->openf[fileID] == NULL || fileSystem->openf[fileID]->type == 1) {
		printf("Khong the ghi!!!\n");
		machine->WriteRegister(2, -1);
	} else {
		oldPos = fileSystem->openf[fileID]->getSeekPosition(); 
		buffer = User2System(virtualAddr, charcount);
		if (fileID == 1) { // stdout
			int i = 0;
			while (buffer[i] != 0) // Vong lap de write den khi gap ky tu '\n'
			{
				synchConsole->Write(buffer + i, 1); // Su dung ham Write cua lop SynchConsole 
				i++;
			}
			machine->WriteRegister(2, i - 1);
		} else {
			int i = 0;
			while (buffer[i] != 0) {
				fileSystem->openf[fileID]->Write(buffer + i, 1);
				i++;
			}
			int newPos = fileSystem->openf[fileID]->getSeekPosition();
			machine->WriteRegister(2, newPos - oldPos);
		} 
		delete buffer;
	}
}

// Ham xu ly syscall Exec
void ExecHandler(){
	int virtAddr;
	virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
	char* name;
	name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1); // Lay ten chuong trinh, nap vao kernel
		
	if(name == NULL)
	{
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		machine->WriteRegister(2, -1);
		return;
	}
	OpenFile *oFile = fileSystem->Open(name);
	if (oFile == NULL)
	{
		printf("\nExec:: Can't open this file.");
		machine->WriteRegister(2,-1);
		increasePC();
		return;
	}
	delete oFile;
	// Return child process id
	int id = pTab->ExecUpdate(name); 
	machine->WriteRegister(2,id);
	delete[] name;	
	increasePC();
	return;
}


// Xu ly syscall Join
void JoinHandler() {
	int id = machine->ReadRegister(4);
	int res = pTab->JoinUpdate(id);
	machine->WriteRegister(2, res);
	increasePC();
	return;
}

// Xu ly syscall Exit
void ExitHandler(){
	int exitStatus = machine->ReadRegister(4);
	if(exitStatus != 0)
	{
		increasePC();
		return;
			
	}			
			
	int res = pTab->ExitUpdate(exitStatus);
	machine->WriteRegister(2, res);

	currentThread->FreeSpace();
	currentThread->Finish();
	increasePC();
	return;
}

// Xu ly syscall CreateSemaphore
void CreateSemaphoreHandler(){
	// int CreateSemaphore(char* name, int semval).
	int virtAddr = machine->ReadRegister(4);
	int semval = machine->ReadRegister(5);
	char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
	if(name == NULL)
	{
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;
	}
			
	int res = semTab->Create(name, semval);
	if(res == -1)
	{
		DEBUG('a', "\n Khong the khoi tao semaphore");
		printf("\n Khong the khoi tao semaphore");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;				
	}
			
	delete[] name;
	machine->WriteRegister(2, res);
	increasePC();
	return;
}

void DownHandler(){
	int virtAddr = machine->ReadRegister(4);

	char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
	if(name == NULL)
	{
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;
	}
			
	int res = semTab->Wait(name);
	if(res == -1)
	{
		DEBUG('a', "\n Khong ton tai ten semaphore nay!");
		printf("\n Khong ton tai ten semaphore nay!");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;				
	}
			
	delete[] name;
	machine->WriteRegister(2, res);
	increasePC();
	return;
}

void UpHandler(){
	int virtAddr = machine->ReadRegister(4);

	char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
	if(name == NULL)
	{
		DEBUG('a', "\n Not enough memory in System");
		printf("\n Not enough memory in System");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;
	}
			
	int res = semTab->Signal(name);
	if(res == -1)
	{
		DEBUG('a', "\n Khong ton tai ten semaphore nay!");
		printf("\n Khong ton tai ten semaphore nay!");
		machine->WriteRegister(2, -1);
		delete[] name;
		increasePC();
		return;				
	}
			
	delete[] name;
	machine->WriteRegister(2, res);
	increasePC();
	return;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException: // Everything ok!
        // return permission to operating system
        interrupt->setStatus(SystemMode);
        DEBUG('a', "Return permission to operating system\n");
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
            DEBUG('a', "Shutdown, initiated by user program.\n");
            printf("Shutdown, initiated by user program.\n");
            interrupt->Halt();
            return;

        case SC_ReadInt:
            return ReadIntHandler();
            
        case SC_PrintInt:
			return PrintIntHandler();

        case SC_ReadChar:
            return ReadCharHandler();

        case SC_PrintChar:
            return PrintCharHandler();

        case SC_ReadString:
            ReadStringHandler();
            increasePC();
            return;

        case SC_PrintString:
            PrintStringHandler();
            increasePC();
            return;
		
	case SC_CreateFile:
		CreateFileHandler();
		increasePC();
		return;
		
	case SC_Open:
		OpenHandler();
		increasePC();
		return;
		
	case SC_Close:
		CloseHandler();
		increasePC();
		return;
		
	case SC_Read:
		ReadHandler();
		increasePC();
		return;
		
	case SC_Write:
		WriteHandler();
		increasePC();
		return;
		case SC_Exec:
	    return ExecHandler();
	case SC_Join:
	    return JoinHandler();
	case SC_Exit:
	    return ExitHandler();
	case SC_CreateSemaphore:
	    return CreateSemaphoreHandler();
	case SC_Up:
	    return UpHandler();
	case SC_Down:
	    return DownHandler();
			
        default:
            printf("Unexpected system call type %d\n", type);
        }
        break;

    default:
        printf("Unexpected user mode exception %d %d\n", which, type);
    }

    ASSERT(FALSE);
}
