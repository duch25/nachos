#include "syscall.h"
#define MAX_LENGTH 255
int main()
{
	char filename[MAX_LENGTH];
	int stdin = Open("stdin", 2);
	int len, status;
	int fileID;
	if (stdin == 0) {
		PrintString("Nhap ten file ban muon tao: ");
		len = Read(filename, MAX_LENGTH, stdin);
		PrintString("Ban vua nhap: ");
		PrintString(filename);
		PrintString("\n");
		
		status = CreateFile(filename);
		if (status == 0) {
			PrintString("Tao file thanh cong !!!");
			fileID = Open(filename, 0);
			status = Write("Bach\nTran\nGia", 255, fileID);
			fileID = Open("stdout", 3);
			status = Write("Bach\nTran\nGia", 255, fileID);
			
		} else {
			PrintString("Tao file khong thanh cong !!!");
		}
		
		Close(stdin);
	}

	Halt();
}
