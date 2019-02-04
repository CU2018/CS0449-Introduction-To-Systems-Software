//Siyu Zhang
//siz24
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE* f = fopen(argv[1], "r");  //open the file as read only
	if (f == NULL)
	{
		printf("The file cannot be opened.\n");
		return 1;
	}
	//get the length of the file
	fseek(f, 0, SEEK_END);
	int flength = ftell(f);
	fseek(f, 0, SEEK_SET);
	 
	int offset = 0;  //offset: start from every char in the file;
	int count = 0;   //count: the number of printable char at current offset

	while(offset < flength)	//loop from the first offset till the end of the file
	{
		char curr; //current char at this offset
		fread(&curr, sizeof(char), 1, f); //read the first char from the file, now the position of the file is 1
		offset = ftell(f);//update the offset, same as offset++
		
		int begin = offset - 1; 	//the original offset(fread makes the offset add 1)
		
		while(curr >= 32 && curr <= 126)  //find the printable char
		{
			count++;
			fread(&curr, sizeof(char), 1, f); //read the next char from the file
		}
		int end = ftell(f); //finish finding all the possible sequences from the current offset
		if (count >= 4)
		{
			int string_len = end - begin;  //the length of the string (including the '\0')
			fseek(f, begin, SEEK_SET); //go to the offset of this loop
			char string[string_len]; //an array of char
			fread(&string, sizeof(string), 1, f); //read from the offset of the file and copy the chars into the array
			string[string_len - 1] = '\0';  //for printing the string
			printf("%s\n", string);
		}
		count = 0; //set count back to 0 and go to a new iteration for the next offset
	}
	
	fclose(f);
	return 0;
}