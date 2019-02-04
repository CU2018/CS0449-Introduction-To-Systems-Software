//siz24

#include <stdio.h>
#include <unistd.h>

#include "extramalloc.h"

// Ooooh, pretty colors...
#define CRESET   "\e[0m"
#define CRED     "\e[31m"
#define CGREEN   "\e[32m"
#define CYELLOW  "\e[33m"
#define CBLUE    "\e[34m"
#define CMAGENTA "\e[35m"
#define CCYAN    "\e[36m"

// You can use these in printf format strings like:
//        printf("This is " RED("red") " text.\n");
// NO COMMAS!!

#define RED(X)     CRED X CRESET
#define GREEN(X)   CGREEN X CRESET
#define YELLOW(X)  CYELLOW X CRESET
#define BLUE(X)    CBLUE X CRESET
#define MAGENTA(X) CMAGENTA X CRESET
#define CYAN(X)    CCYAN X CRESET

void* start_test(const char* where) 
{
	printf("-----------------------------------------------------------------------------\n");
	printf(RED("Running %s...\n"), where);
	return sbrk(0);
}

void check_heap_size(const char* where, void* heap_at_start) 
{
	void* heap_at_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_at_end - heap_at_start);

	if(heap_size_diff)
		printf(RED("After %s the heap got bigger by %u (0x%X) bytes...\n"),
			where, heap_size_diff, heap_size_diff);
	else
		printf(GREEN("Yay, after %s, everything was freed!\n"), where);
}

void fill_array(int* arr, int length) 
{
	int i;

	for(i = 0; i < length; i++)
		arr[i] = i + 1;
}

int* make_array(int length) 
{
	int* ret = quick_malloc(sizeof(int) * length);
	fill_array(ret, length);
	return ret;
}

void test_writing() 
{
	void* heap_at_start = start_test("test_writing");

	int* a = make_array(10);
	int* b = make_array(10);

	// Just to make sure..
	int i;
	for(i = 0; i < 10; i++)
		printf(YELLOW("a[%d] = %d, b[%d] = %d\n"), i, a[i], i, b[i]);
	
	print_list();

	// Freeing in reverse order.
	printf(GREEN("Free the last allocated block...\n"));
	quick_free(b);
	print_list();
	quick_free(a);

	check_heap_size("test_writing", heap_at_start);
}

void test_reuse() 
{
	void* heap_at_start = start_test("test_reuse");
	int* a = make_array(20);
	int* b = make_array(20);
	quick_free(a);
	print_list();

	int* c = make_array(20);
	if(a == c)
		printf(GREEN("\nYou reused the free block.\n\n"));
	print_list();

	quick_free(c);
	quick_free(b);

	check_heap_size("test_reuse", heap_at_start);
}

void test_quick_fit()
{
	void* heap_at_start = start_test("test_quick_fit");
	
	int* a = make_array(10);  
	int* b = make_array(22);  
	quick_free(a);//should go to small
	int* c = make_array(40);    
	int* hold = make_array(12);
	quick_free(c);//should go to large

	print_list();
	
	int* small = make_array(4);
	if (a == small)
		printf(GREEN("\nthe small one goes to the small bin.\n\n"));
	print_list();
	int* large = make_array(33);
	if (c == large)
		printf(GREEN("\nthe large one goes to the large bin.\n\n"));
	quick_free(b);//should go to medium
	print_list();
	int* medium = make_array(16);
	if (b == medium)
		printf(GREEN("\nthe medium one goes to the medium bin.\n\n"));
	print_list();

	quick_free(medium);//should go to medium
	quick_free(small);
	quick_free(large);
	quick_free(hold);

	
	
	check_heap_size("test_reuse", heap_at_start);
}

void test_coalescing() 
{
	void* heap_at_start = start_test("test_coalescing");
	int* a = make_array(10);
	int* b = make_array(10);
	int* c = make_array(10);
	int* d = make_array(10);
	int* e = make_array(10);
	int* more = make_array(10);
	
	quick_free(a); // 40B
	quick_free(b); // 100B
	quick_free(c); // 160B
	quick_free(d); // 220B
	quick_free(e); // 280B
	// This should reuse a's block, since it's 280 bytes.
	print_list();
	int* f = make_array(52);

	if(a == f)
		printf(GREEN("\nYou reused the coalesced block.\n\n"));
	print_list();


	// Now, when we free these, they should coalesce into
	// a single big block, and then be sbrk'ed away!
	quick_free(f);
	quick_free(more);

	check_heap_size("part 1 of test_coalescing", heap_at_start);

	// Let's re-allocate...
	a = make_array(10);
	b = make_array(10);
	c = make_array(10);
	d = make_array(10);
	e = make_array(10);

	// Now let's test freeing random blocks. my_free(a)
	// will test freeing with a single next free neighbor, and
	// my_free(c) will test with two free neighbors.

	// After each you should have:
	quick_free(b); // one free 40B
	quick_free(d); // two free 40B
	quick_free(a); // one free 96B, one free 40B
	quick_free(c); // one free 208B
	quick_free(e); // nothing left!

	check_heap_size("part 2 of test_coalescing", heap_at_start);

	// Finally, let's make sure coalescing at the beginning
	// and end of the heap work properly.

	a = make_array(10);
	b = make_array(10);
	c = make_array(10);
	d = make_array(10);
	e = make_array(10);

	// After each you should have:
	quick_free(b); // one free 40B, four used 40B
	quick_free(a); // one free 96B, three used 40B
	quick_free(d); // one free 96B, one free 40B, two used 40B
	quick_free(e); // one free 96B, one used 40B
	quick_free(c); // nothing left!

	check_heap_size("part 3 of test_coalescing", heap_at_start);
}

// Makes sure that your block splitting works.
void test_splitting() 
{
	void* heap_at_start = start_test("test_splitting");

	int* medium = make_array(80); // make a 320-byte block.

	int* holder = make_array(4);  // holds the break back.

	quick_free(medium);


	// Now there should be a free 320-byte block.
	int* too_big = make_array(57);  //used 232-byte, 68 bytes left, split it and free one goes to medium bin
	quick_free(too_big);
	print_list();
	
	int* tiny1 = make_array(40); //go to the large bin and split one free to medium

	if(tiny1 == medium)
		printf(GREEN("\nYou split the 320B block\n\n"));
	print_list();

	quick_free(tiny1);

	quick_free(holder);
	
	check_heap_size("test_splitting", heap_at_start);
}

void test_remove()
{
	void* heap_at_start = start_test("test_remove from bins\n");

	int* a = make_array(80); //320 bytes
	int* b = make_array(4);  //16 byte
	quick_free(a); //should go large bin
	
	int* c = make_array(18);  //72 byte
	int* d = make_array(8);  //16 byte
	quick_free(c); //should go medium bin
	
	int* e = make_array(8);
	int* f = make_array(8);  
	quick_free(e);  //should go small bin

	print_list();
	
	int* small = make_array(8);  //should reuse small
	if(e == small)
		printf(GREEN("\nYou reused small bin.\n\n"));
	
	print_list();
	
	int* large = make_array(80);  //should reuse large
	if(a == large)
		printf(GREEN("\nYou reused large bin\n\n"));
	
	print_list();
	
	int* medium = make_array(18);  //should reuse medium
	if(c == medium)
		printf(GREEN("\nYou reused medium bin.\n"));
	
	print_list();
	
	int* g = make_array(4); 
	
	
	
	quick_free(b);
	quick_free(d);
	quick_free(f);
	
	int* h = make_array(4); 
	quick_free(h);
	
	quick_free(small);
	quick_free(medium);
	quick_free(large);
	quick_free(g);
	
	check_heap_size("test_remove", heap_at_start);
}

int main()
{
	// You can use sbrk(0) to get the current position of the break.
	// This is nice for testing cause you can see if the heap is the same size
	// before and after your tests, like here.
	void* heap_at_start = sbrk(0);
	test_writing();
	test_reuse();
	test_quick_fit();
	test_coalescing();
	test_remove();
	test_splitting();

	// Just to make sure!
	check_heap_size("main", heap_at_start);
	return 0;
	
}
