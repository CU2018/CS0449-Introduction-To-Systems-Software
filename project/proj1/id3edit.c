//Siyu Zhang
//siz24
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct ID3_TAG
{
	char tag[3];  // 0-2
	char title[30];  //3-32
	char artist[30];  //33-62
	char album[30];  //63-92
	char year[4];  //93-96
	char comment[28];  //97-124
	char zero; //125
	unsigned char track; //126, non-negative
	char genre; //127, set 0
}ID3_TAG;

FILE* open_file(char* filename)
{
	FILE* f = fopen(filename, "rb+");
	if (f == NULL)
	{
		printf("The file cannot be opened.\n");
		exit(1);
	}
		
	return f;
}

void help_info()
{
	printf("Hello, this is an ID3 editor. Here are the instructions to use the program: \n");
	printf("1. You can enter a MP3 FILENAME after './id3edit' to check whether it has an ID3tag, like './id3edit FILENAME ';\n");
	printf("2. If it has one ID3 tag, you can read and display the contents of the MP3 file,including the content of its Title, Artist, Album, Year, Comment, and Track;\n");
	printf("3. You can also set the content of the field if you want by typing: './id3edit FILENAME -field value'; \n");
	printf("4. If it doesn't have one and you want to set the fields of it, this program will create an ID3 tag for it and you can do the same as the instruction above;\n");
	printf("5. This program allows you to set any number of fields at once. You could just typing'./id3edit FILENAME -field value -field value -field value -field value' etc.;\n");
	printf("6. Please use LOWERCASE when type in.\n");
	printf("Now, you can play with it! OwO\n");
}

int has_tag(ID3_TAG* id3_tag)
{
	return strncmp(id3_tag->tag, "TAG", sizeof(id3_tag->tag)) == 0;
}

void add_tag(FILE* f, ID3_TAG* id3_tag)
{
	memset(id3_tag, 0,sizeof(ID3_TAG));
	strncpy(id3_tag->tag, "TAG", sizeof(id3_tag->tag));
	fseek(f, -128, SEEK_END);
	fwrite(id3_tag, sizeof(ID3_TAG), 1, f);
	
	printf("Finish adding!\n");
}

void show_content(ID3_TAG* id3_tag)
{
	printf("Title: %.*s\n", (int)sizeof(id3_tag->title), id3_tag->title);
	printf("Artist: %.*s\n",  (int)sizeof(id3_tag->artist), id3_tag->artist);
	printf("Album: %.*s\n", (int)sizeof(id3_tag->album), id3_tag->album);
	printf("Year: %.*s\n", (int)sizeof(id3_tag->year), id3_tag->year);
	printf("Comment: %.*s\n", (int)sizeof(id3_tag->comment), id3_tag->comment);
	printf("Track: %.*d\n", (int)sizeof(id3_tag->track), (signed)id3_tag->track);
}

int is_even(int argc)
{
	return (argc%2 == 0); //is even
}

void read_tag(FILE* f, ID3_TAG* id3_tag, int flag)
{
	fseek(f, -128, SEEK_END);
	fread(id3_tag, sizeof(ID3_TAG), 1, f);
	
	if (has_tag(id3_tag))
	{
		printf("Here are the contents of this MP3 file:\n");
		show_content(id3_tag);
	}
	else if (flag == 0)
		printf("This MP3 file doesn't have an ID3 tag. You can try to set the fileds of it directly since this program will add a tag for it.\n");
	else //add a tag when during setting
	{
		printf("This MP3 file doesn't have an ID3 tag. Adding a tag...\n");
		add_tag(f, id3_tag);
	}
}

int set_field(char* field, char* value, ID3_TAG* id3_tag)
{
	int flag = 1;
	printf("\nChanging %s...\n", field);
	if (strncmp(field, "-title", sizeof(value)) == 0)
		strncpy(id3_tag->title, value, sizeof(id3_tag->title));
	else if (strncmp(field, "-artist", sizeof(value)) == 0)
		strncpy(id3_tag->artist, value, sizeof(id3_tag->artist));
	else if (strncmp(field, "-album", sizeof(value)) == 0)
		strncpy(id3_tag->album, value, sizeof(id3_tag->album));
	else if (strncmp(field, "-year", sizeof(value)) == 0)
		strncpy(id3_tag->year, value, sizeof(id3_tag->year));
	else if (strncmp(field, "-comment", sizeof(value)) == 0)
		strncpy(id3_tag->comment, value, sizeof(id3_tag->comment));
	else if (strncmp(field, "-track", sizeof(value)) == 0)
		id3_tag->track = (char)atoi(value);
	else
	{
		printf("Wrong filed name! You can only modify six parts by typing '-title' '-artist' '-album' '-year' '-comment' '-track' and don't forget to add the value after each of them.\n");
		flag = 0;
	}
	return flag;
}

int main(int argc, char** argv)
{
	ID3_TAG id3_tag;
	
	if (argc == 1) //meaning only tpyed to run the program
	{
		help_info();
	}
	else if (argc == 2) //enter the MP3 filename
	{
		FILE* f = open_file(argv[1]);
		
		read_tag(f, &id3_tag, 0); //check whether there is a tag
		
		fclose(f);
	}
	else if (argc > 2 && is_even(argc)) //check whether arguments are complete
	{
		FILE* f = open_file(argv[1]);
		
		read_tag(f, &id3_tag, 1);  //check whether there is a tag; if no tags, add one
		
		int num_field = argc/2 - 1;
		int i = 2;
		while (num_field != 0)
		{
			if (!set_field(argv[i], argv[i+1], &id3_tag))
				return 1;
			num_field--;
			i+=2;
		}

		fwrite(&id3_tag, sizeof(ID3_TAG), 1, f);
		printf("\nAfter Change: \n");
		show_content(&id3_tag);
		
		fclose(f);
	}
	else  //not enough arguments
	{
		printf("The arguments are incomplete. You might lose a field or a value.\n");
	}
		
		
	return 0;
}

