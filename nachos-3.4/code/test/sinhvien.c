#include "syscall.h"

char contentFile[1000];

void WriteDataIntoOutput(char data[], int len) {
	int outputID, status, index, i;
	char c;
	outputID = Open("output.txt", 0);
	
	index = 0;
	while (1) {
		status = Read(&c, 1, outputID);
		if (status == -2) { // EOF
			break;
		}
		contentFile[index] = c;
		index++;
	}
	Write(data, len, outputID);
	
	Close(outputID);
}

int main()
{
    int inputID, status, index, i;
    char c, data[10];
    
    
    inputID = Open("input.txt", 1);
    index = 0;
    
    while (1) {
		Read(&c, 1, inputID);
		if (c == '\n') break;
	}
    
    status = CreateFile("output.txt");
    
    while (1) {
		
		status = Read(&c, 1, inputID);
		if (status == -2) { // EOF
			Down("sinhvien");
			WriteDataIntoOutput(data, index);
			Up("voinuoc");
			break;
		}
		if (c == '\n') {
			//data[index] = '*';
			//index++;
			Down("sinhvien");
			WriteDataIntoOutput(data, index);
			Up("voinuoc");
			
			index = 0;
			data[index] = '\n';
			index++;
		} else if (c == ' ') {
			Down("sinhvien");
			WriteDataIntoOutput(data, index);
			Up("voinuoc");
			index = 0;
		} else {
			data[index] = c;
			index++;
		}
	}
	
    
}
