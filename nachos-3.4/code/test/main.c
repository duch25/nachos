#include "syscall.h"

int main()
{
	int sinhvienPID;
	
    CreateSemaphore("sinhvien", 1);
    CreateSemaphore("voinuoc", 0);
    
    sinhvienPID = Exec("./test/sinhvien");
    Exec("./test/voinuoc");
    Join(sinhvienPID);
    
}
