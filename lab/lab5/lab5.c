#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*PREDICATE)(const void*);

// ------------------------------------------------------

int filter(void* output, const void* input, int length, int item_size, PREDICATE pred)
{
	int count = 0;
	
	char* input_pointer = (char*) input;
	char* output_pointer = (char*) output;
	
	for (int i = 0; i < length; i++)
	{
		if (pred(input)) 
		{
			memcpy(output, input, item_size);
			count++;
			input_pointer += item_size;
			output_pointer += item_size;
			input = (void*) input_pointer;
			output = (void*) output_pointer;
		}
		else
		{
			input_pointer += item_size;
			input = (void*) input_pointer;
		}
	}
	
	return count;
}

int less_than_50(const void* p)
{
	//cast the arguement into a float pointer and then access and compare
	float p_value = *(const float*)p;
	return (p_value<50);
}

// ------------------------------------------------------
// you shouldn't have to change the stuff below here.
// you can for testing, but please put it back the way it was before you submit.

#define NUM_VALUES 10

float float_values[NUM_VALUES] =
{
	31.94, 61.50, 36.10,  1.00,  6.35,
	20.76, 69.30, 19.60, 79.74, 51.29,
};

int main()
{
	float filtered[NUM_VALUES];
	int filtered_len = filter(filtered, float_values, NUM_VALUES, sizeof(float), &less_than_50);

	printf("there are %d numbers less than 50:\n", filtered_len);

	for(int i = 0; i < filtered_len; i++)
		printf("\t%.2f\n", filtered[i]);

	return 0;
}