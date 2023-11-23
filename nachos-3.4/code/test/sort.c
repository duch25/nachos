#include "syscall.h"

void swapElements(int a[], int i, int j) {
	int tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

void sortAscending(int a[], int n) {
	int i, j, tmp;
	
	for(i = 0; i < n; i++) {
		for(j = i + 1; j < n; j++) {
			if(a[i] > a[j]) {
				swapElements(a, i, j);			
			}		
		}
	}
}


void sortDescending(int a[], int n) {
	int i, j, tmp;
	
	for(i = 0; i < n; i++) {
		for(j = i + 1; j < n; j++) {
			if(a[i] < a[j]) {
				swapElements(a, i, j);			
			}		
		}
	}
}

int main() {
	int n, i, order;
	int a[100];
	
	PrintString("Enter number of elements: ");
	n = ReadInt();

	while(n <= 0 || n > 100) {
		PrintString("Invalid number! Enter again: ");
		n = ReadInt();
	}

	for(i = 0; i < n; i++) {
		PrintString("Enter element ");
		PrintInt(i);
		PrintString(" th: ");		
		a[i] = ReadInt();	
	}

	PrintString("Choose type of sort\n0. Ascending.\n1. Descending.\n");
	order = ReadInt();

	if(order == 0) {
		sortAscending(a, n);
	}
	else if(order == 1) {
		sortDescending(a, n);
	}
	 
	PrintString("Result: ");

	for(i = 0; i < n; i++) {
		PrintInt(a[i]);
		PrintString(" ");
	
		if(i == n - 1)	
			PrintString("\n");	
	}

	Halt();
}
