#include "syscall.h"

int main()
{
	int sinhvienPID;

    CreateSemaphore("sinhvien", 1);
    CreateSemaphore("voinuoc", 0);
    CreateSemaphore("main", 0);
    sinhvienPID = Exec("./test/sinhvien");
    Exec("./test/voinuoc");
    Down("main");
    //Join(sinhvienPID);
    PrintString("Program End");
}
