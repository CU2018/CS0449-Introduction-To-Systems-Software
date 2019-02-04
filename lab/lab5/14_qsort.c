#include <stdio.h>
#include <stdlib.h>

// qsort calls this function with pointers to the two items to compare.
// you have to cast the pointers to access the data at those pointers.
int compare_ints(const void* pa, const void* pb)
{
	// casting pointer and then dereferencing to interpret the data there.
	int a = *(const int*)pa;
	int b = *(const int*)pb;
	// uncomment the following line to see fun stuff happen!
	// printf("qsort is comparing %d and %d.\n", a, b);

	// this works... think about how it works. what will it return
	// when a is less than b? when it's greater? when they're equal?
	return a - b;
}

// another comparison function to try!
int compare_ints_reverse(const void* pa, const void* pb)
{
	int a = *(const int*)pa;
	int b = *(const int*)pb;
	return b - a;
}

#define ARR_LENGTH 8

int main()
{
	int array[ARR_LENGTH] = {55, 209, 1, 17, -2, 37, 84, 20049};

	// first parameter is the array to sort
	// second is how many ITEMS the array is
	// third is how big EACH ITEM is, in bytes
	// fourth is a pointer to the comparison function
	qsort(array, ARR_LENGTH, sizeof(array[0]), &compare_ints_reverse);

	for(int i = 0; i < ARR_LENGTH; i++)
		printf("array[%d] = %d\n", i, array[i]);

    return 0;
}