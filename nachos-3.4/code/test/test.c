#include "syscall.h"


int main() {
	int n;
	CreateFile("testCreateFile.txt");	
	n = Open("testCreateFile.txt", 0);
	if (n != -1) {
		PrintString("Mo file thanh cong!\n");
	} else {
		PrintString("Mo file khong thanh cong!\n");
	}
	
	n = Close(n);
	if (n != -1) {
		PrintString("Dong file thanh cong!\n");
	} else {
		PrintString("Dong file khong thanh cong!\n");
	}
	Halt();
}
