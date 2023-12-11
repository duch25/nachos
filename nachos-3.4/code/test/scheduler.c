#include "syscall.h"

void main()
{
    int pingPID, pongPID;
    CreateSemaphore("ping", 1);
    CreateSemaphore("pong", 0);
     
    pingPID = Exec("./test/ping");
    pongPID = Exec("./test/pong");
	
	Join(pingPID);
}
