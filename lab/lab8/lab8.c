#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS 8

// =================================================================================================
//
//
// Scroll down to the bottom!!!
// Don't change anything here!!!
//
//
// =================================================================================================

typedef struct {
	int* data;         // the data to search through.
	int start;         // this thread's starting index.
	int end;           // this thread's ending index.
	int value_to_find; // the value being searched for.
	int found_index;   // if a thread found the value, the index it found it at; else -1.
} SearchContext;

void fatal(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void fatal_errno(const char* msg, int error) {
	fprintf(stderr, "%s\n", msg);
	errno = error;
	perror("reason");
	exit(1);
}

int check_args(int argc, char** argv) {
	if(argc < 3) {
		fatal("run program like\n\t./lab8 data_file value_to_find");
	}
	return atoi(argv[2]);
}

int* read_data_file(const char* filename, unsigned int* pLen) {
	// open file
	FILE* f = fopen(filename, "rb");
	if(f == NULL) {
		fatal_errno("could not open data file", errno);
	}

	// get length
	fseek(f, 0, SEEK_END);
	unsigned int byte_len = ftell(f);
	fseek(f, 0, SEEK_SET);

	if((byte_len % sizeof(int)) != 0) {
		fatal("data length is not a multiple of sizeof(int).");
	}

	// read whole file into heap array
	int* ret = malloc(byte_len);
	fread(ret, byte_len, 1, f);
	fclose(f);

	// pLen is the length *in items*
	*pLen = byte_len / sizeof(int);
	return ret;
}

void set_up_contexts(SearchContext* ctxs, int* arr, unsigned int len, int value_to_find) {
	int stride = len / NUM_THREADS;

	// give each thread a slice of the array.
	for(int i = 0; i < NUM_THREADS; i++) {
		ctxs[i] = (SearchContext){
			.data = arr,
			.start = i * stride,
			.end = (i + 1) * stride,
			.value_to_find = value_to_find,
			.found_index = -1, // didn't find it.
		};
	}

	// in case the length doesn't divide evenly by the number of threads,
	// the last thread will pick up the remainder.
	ctxs[NUM_THREADS - 1].end = len;
}

void make_threads(pthread_t* tids, SearchContext* ctxs);
void wait_for_threads(pthread_t* tids);
void display_results(SearchContext* ctxs, int value_to_find);

int main(int argc, char** argv) {
	int value_to_find = check_args(argc, argv);

	unsigned int len;
	int* arr = read_data_file(argv[1], &len);

	SearchContext ctxs[NUM_THREADS];
	set_up_contexts(ctxs, arr, len, value_to_find);

	pthread_t tids[NUM_THREADS];
	make_threads(tids, ctxs);
	wait_for_threads(tids);
	display_results(ctxs, value_to_find);

	free(arr);
	return 0;
}

// =================================================================================================
// You will write your code below this line.
// =================================================================================================

void* thread_main(void* arg) {
	// cast 'arg' to a SearchContext* variable.
	// go look at the definition of SearchContext to see what's in it!
	// search through the appropriate array slots (start inclusive, end noninclusive).
	// if you find the value, set the context's found_index to that index and return.
	// if you don't find the value, just return. (set_up_contexts already set found_index to -1.)
	SearchContext* ctxs = (SearchContext*)arg;
	int* search_array = ctxs->data;
	long i;
	for (i = ctxs->start; i < ctxs->end; i++)
	{
		if (search_array[i] == ctxs->value_to_find)
		{
			ctxs->found_index = i;
			break;
		}
	}
}

void make_threads(pthread_t* tids, SearchContext* ctxs) {
	// make NUM_THREADS threads, and put the thread IDs into the tids array.
	// use thread_main as the thread's main function, and give each thread its
	// own context from the ctxs array (ctxs[i]).
	long i;
	for (i = 0; i < NUM_THREADS; i++)
		pthread_create(&tids[i], NULL, &thread_main, (void*) &ctxs[i]);
}

void wait_for_threads(pthread_t* tids) {
	// pthread_join all the threads in tids.
	int i;
	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(tids[i], NULL);
}

void display_results(SearchContext* ctxs, int value_to_find) {
	// look at all the contexts. if one of them has a found_index != -1,
	// that means that thread found it; print the thread number and where it found it.
	// if none of the threads found the value, say so.
	int i;
	long found = -1;
	for (i = 0; i < NUM_THREADS; i++)
	{
		SearchContext ctx = ctxs[i];
		if (ctx.found_index != -1)
		{
			found = ctx.found_index;
			break;
		}	
	}
	if (found != -1)
		printf("thread %d found %d at index %d\n", i, value_to_find, found);
	else
		printf("value %d was not found.\n", value_to_find);
}