#include <stdio.h>
#include <string.h>

// This example shows how to use the strtok function to split a string
// into pieces.

int main()
{
	// strtok MODIFIES THE STRING TO PARSE. So you can't give it a string
	// literal.
	char string_to_parse[] = "this is a string:to::split.:::";
	int num_tokens = (strlen(string_to_parse) / 2) + 1; // worst case...
	char* tokens[num_tokens];

	// Uncomment this line and the for loop at the end to see what strtok
	// actually did to string_to_parse's memory.
	// int orig_len = strlen(string_to_parse);

	const char* delim = " :";
	char* token = strtok(string_to_parse, delim);
	int i;
	for(i = 0; token != NULL; i++)
	{
		tokens[i] = token;
		token = strtok(NULL, delim);
	}

	tokens[i] = NULL; // make sure to NULL out the last slot in the array

	for(i = 0; tokens[i] != NULL; i++)
		printf("token %d: '%s'\n", i, tokens[i]);

	// for(i = 0; i < orig_len; i++)
	// 	printf("%d: %c (%d)\n", i, string_to_parse[i], string_to_parse[i]);

	return 0;
}