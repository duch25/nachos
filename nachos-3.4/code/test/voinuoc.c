#include "syscall.h"

char contentFile[1000];
char data[10];

int main()
{	
	int outputID, index, status, i, swID, num, id, v1, v2;
	char ans;
	char c;
	v1 = 0;
	v2 = 0;
	while (1) {
		Down("voinuoc");
		swID = 0;
		index = 0;

		outputID = Open("output.txt", 0);
		if(outputID == -1) {
			Up("main");
			break;		
		}
		while (1) {
			status = Read(&c, 1, outputID);
			if (status == -2) {
				break;
			}
			if (c == ' ') {
				swID = index;
			}
			contentFile[index] = c;
			index++;
		}

		num = 0;
		id = 0;
		if (!swID) {
			swID--;
		}

		for (i = swID + 1; i < index; i++) {
			if (contentFile[i] == '\n') {
				v1 = v2 = 0;
				continue;
			}
			num = num * 10 + (contentFile[i] - 48);
			data[id] = contentFile[i];
			id++;
		}

		if(num != 0) {
			if (v1 == v2) {
				ans = '1';
				v1 = num;
				v2 = 0;
			} else if (v1 > v2) {
				ans = '2';
				v1 -= v2;
				v2 = num;
			} else {
				ans = '1';
				v2 -= v1;
				v1 = num;
			}

		}

		Write(" ", 1, outputID);
		Write(&ans, 1, outputID);
		Write("  ", 2, outputID);


		Close(outputID);
		Up("sinhvien");
	}
}
