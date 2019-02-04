#include <stdio.h>
#include <stdlib.h>

void fun();

int main() {
	int x;
	char buf[30];

	printf("Enter a number: ");
	fgets(buf, sizeof(buf), stdin);
	sscanf(buf, "%d", x);

	fun();
	less_fun();
	
	return 0;
}

void fun() {
	int a = 5;
	int b = 0;
	int c = a / b;
	printf("Result is %d\n", c);
}

void less_fun() {
	int* p = malloc(sizeof(int));
	*p = 45;
	int* q = *p;
	if(q != NULL) {
		printf("*q = %d\n", *q);
	}
}
