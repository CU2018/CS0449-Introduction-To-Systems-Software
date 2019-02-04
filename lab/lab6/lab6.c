//Siyu Zhang
//siz24
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h> //for loading zlib library

//loading zlib and the needed functions
unsigned long (*compressBound)(unsigned long length);
int (*compress)(void *dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
int (*uncompress)(void *dest, unsigned long* destLen,const void* source, unsigned long sourceLen);

FILE* open_file(char* filename)
{
	FILE* f = fopen(filename, "rb+");
	if (f == NULL)
	{
		printf("Error: The file cannot be opened.\n");
		exit(1);
	}
		
	return f;
}

int check_argc(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}

unsigned long get_len(FILE* f)
{
	unsigned long len;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	return len;
}

int main(int argc, char** argv)
{
	//not enough arguments typed on the cmd
	if (argc < 3)
	{
		printf("Too few program arguments! We need at least 3!\n");
		return 1;
	}
	
	//load the zlib library
	void* lib = dlopen("libz.so", RTLD_NOW);
	if (lib == NULL)
	{
		printf("Error: Couldn't load the library!\n");
		return 1;
	}
	
	//extract three functions from zlib library
	compressBound = dlsym(lib, "compressBound");
	if (compressBound == NULL)
	{
		printf("Error: Couldn't load the compressBound function!\n");
		return 1;
	}
	compress = dlsym(lib, "compress");
	if (compress == NULL)
	{
		printf("Error: Couldn't load the compress function!\n");
		return 1;
	}
	uncompress = dlsym(lib, "uncompress");
	if (uncompress == NULL)
	{
		printf("Error: Couldn't load the uncompress function!\n");
		return 1;
	}
	
	//open file argv[2] for binary reading
	FILE* f = open_file(argv[2]);
	
	void* compressed_file;  //dest in compress function, source in uncompress function
	void* uncompressed_file;  //dest in uncompress function, source in compress function
	unsigned long compressed_len;
	unsigned long uncompressed_len = get_len(f); //get uncompressed length by fseek and ftell
	
	
	//get compress command
	if (check_argc(argv[1], "-c"))
	{
		//read the entire file into an input buffer that you malloc
		uncompressed_file = malloc(uncompressed_len);
		fread(uncompressed_file, uncompressed_len, 1, f);

		//malloc a output buffer using compressBound to figure out what size it should be
		compressed_len = compressBound(uncompressed_len);  //get the compressed length
		compressed_file = malloc(compressed_len);
		
		//use compress() to compress the input buffer into the output buffer.
		if (compress(compressed_file, &compressed_len, uncompressed_file, uncompressed_len) != 0)
		{
			printf("Error: Compress failed!\n");
			return -1;
		}
		
		//fwrite three things to stdout:
		fwrite(&uncompressed_len, sizeof(uncompressed_len), 1, stdout);	//the uncompressed size (as an unsigned long)
		fwrite(&compressed_len, sizeof(compressed_len), 1, stdout);//the actual compressed size (as an unsigned long)
		fwrite(compressed_file, compressed_len, 1, stdout);//the output buffer
	}
	//get uncompress command
	else if (check_argc(argv[1], "-d"))
	{
		//fread two things:
		fread(&uncompressed_len, sizeof(uncompressed_len), 1, f);	//the uncompressed size
		fread(&compressed_len, sizeof(compressed_len), 1, f);	//the compressed size
		
		//malloc an input buffer big enough to hold the compressed data
		compressed_file = malloc(compressed_len);
		
		//fread the rest of the data into that buffer (using the compressed size)
		fread(compressed_file, compressed_len, 1, f);
		
		//malloc an output buffer big enough to hold the uncompressed data (using the uncompressed size)
		uncompressed_file = malloc(uncompressed_len);
		
		//use uncompress() to decompress the input buffer into the output buffer
		if (uncompress(uncompressed_file, &uncompressed_len, compressed_file, compressed_len) != 0)
		{
			printf("Error: Uncompress failed!\n");
			return -1;
		}
			
		
		//fwrite the output buffer to stdout
		fwrite(uncompressed_file, uncompressed_len, 1, stdout);
	}
	else 
	{
		printf("Error: the second argument should be '-c' or '-d'!\n");
		return 1;
	}
	fclose(f);
	free(compressed_file);
	free(uncompressed_file);
	return 0;
}







