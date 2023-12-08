#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	char fileName[MAX_LENGTH];
	int fileID = Open("bach.txt", 0);
	Read(fileName, 20, fileID); // Goi ham Read doc ten file vua nhap
	PrintString(fileName);
	PrintString("\n");
	
	Read(fileName, 123, fileID); // Goi ham Read doc ten file vua nhap
	PrintString(fileName);
	Close(fileID);
		
	
	
	
	Halt();
}
