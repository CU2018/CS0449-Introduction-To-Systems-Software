#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

typedef struct header* Header;  //Header is defined as the pointer to the struct header
struct header   //16 bytes in 32 bit (-m32)
{	
	unsigned status; //0 for used and 1 for free
	unsigned data; //the size of the block
	struct header* prev;
	struct header* next;
};

//static global variables: head, tail, last_allocated, only one heap
//can be seen only in this file
static Header heap_head = NULL;
static Header heap_tail = NULL;
static Header last_allocated = NULL;

unsigned int round_up_size(unsigned int data_size)
{
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

static Header set_heap(unsigned int size) //return the heap_head which is a static global variable
{
	heap_head = sbrk(size + sizeof(struct header));  //move the break up

	heap_head->status = 0;  //mark it as used
	heap_head->data = size;
	heap_head->prev = NULL;
	heap_head->next = NULL;
	
	heap_tail = heap_head;
	last_allocated = heap_head;
	
	return heap_head;
}

void split(unsigned int size, Header new_block)
{
	Header remained_free = (void*)new_block + size + sizeof(struct header); //count to the insert block
	
	if( new_block == heap_tail)   //check if the spliting block is the tail
		heap_tail = remained_free;
	
	remained_free->data = new_block->data - size - sizeof(struct header); //get the size of new free block
	remained_free->status = 1;  //mark as free
	
	//insert the remained_free header
	remained_free->prev = new_block;
	remained_free->next = new_block->next;
	new_block->next->prev = remained_free;
	new_block->next = remained_free;
	
	//update the old header
	new_block->status = 0;  //mark as used
	new_block->data = size;
}

Header find_next(unsigned int size, Header start_block, Header end_block)
{
	Header new_block;
	for(new_block = start_block; new_block != end_block; new_block = new_block->next)
	{
		if ( new_block->status == 1 && size == new_block->data)  //no need to split
		{
			new_block->status = 0;  //mark as used
			return new_block;
		}
		if (new_block->status == 1 && (size + sizeof(struct header)) < (new_block->data)) //might need to split the block
		{	
			if (new_block->data - size - sizeof(struct header) >= 16)  //only split when it's not so small
				split(size, new_block);
			else
			{
				//cannot split although leaving internal fragmentations
				new_block->status = 0;  //mark as used
			}
			return new_block;
		}
	}
	return NULL;
}

Header next_fit(unsigned int size)
{
	Header new_block;
	new_block = find_next(size, last_allocated, NULL);  //find the next from last to tail first
	if (new_block != NULL)
		return new_block;
	else  //if not found, start finding from the head till last
	{
		new_block = find_next(size, heap_head, last_allocated);
		if (new_block != NULL)
			return new_block;
		else 
			return NULL;
	}		
}

Header expand_heap(unsigned int size)
{
	Header new_block = sbrk(0);  //sbrk(0) get the current location program break
	
	if (sbrk(sizeof(struct header) + size) == (void*) - 1) //sbrk return(void*) - 1) with error
		return NULL;
	//successfully extend the heap
	new_block->status = 0;
	new_block->data = size;
	new_block->prev = heap_tail;
	new_block->next = NULL;
	//set new tail
	heap_tail->next = new_block;
	heap_tail = new_block;
	//set last_allocated
	last_allocated = new_block;
	
	return new_block;
}

Header combine(Header block1, Header block2)
{
	//assert(block1 != NULL && block2 != NULL);
	//assert(block1->next == block2 && block2->prev == block1);
	block1->data += block2->data + sizeof(struct header);
	
	if (block2 == heap_tail)  //meaning no next
	{
		heap_tail = block1;
	}
	else
	{
		block2->next->prev = block1;
	}
	block1->next = block2->next; 
	return block1;
}

Header coalesce(Header free_header)
{
	if (free_header->prev != NULL && free_header->prev->status == 1) //has free prev
	{
		free_header = combine(free_header->prev, free_header);
	}
	if (free_header->next != NULL && free_header->next->status == 1) //has free next
	{
		return combine(free_header, free_header->next);
	}
	return free_header;
}

void contract(Header free_header)
{
	if (free_header == heap_tail)  //free the tail-->need contract
	{
		if (heap_tail == heap_head)  //free the only block in the heap
		{
			heap_head = NULL;
			heap_tail = NULL;
			last_allocated = NULL;
			brk(free_header);  //move the break down to the free_header
		}
		else 
		{
			if (heap_tail == last_allocated)  //free the last_allocated one which is also the tail
				last_allocated = heap_head;   //reset it to the head
			heap_tail = heap_tail->prev;
			heap_tail->next = NULL;
			brk(free_header);
		}
	}
}

void print_list()
{
	Header h = heap_head;
	int i = 0;

	if (h != NULL)
		printf("Print the list...\n");
	while(h != NULL)
	{
		printf("Header: %d", i);
		printf("  Addr: %p", h);
		printf("  Status: %d", h->status);
		printf("  Data: %d", h->data);
		printf("  Prev: %p", h->prev);
		printf("  Next: %p\n", h->next);
		h = h->next;
		i++;
	}
	printf("heap_head: %p\n", heap_head);
	printf("heap_tail: %p\n", heap_tail);
	printf("last_allocated: %p\n", last_allocated);
	printf("\n");
	
}

void* my_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;
	
	size = round_up_size(size);

	if (heap_head == NULL)	//the first block in the heap
		return (void*)(set_heap(size) + 1);  //return the data part(after the header), have to cast it into a pointer
	else //heap has blocks
	{
		Header new_block = next_fit(size);  //find the next allocate block
		
		if (new_block == NULL)  //the next_fit not found, need extend
			return (expand_heap(size) + 1);   //return the data part(after the header)
		else //find a block to reuse, no need to extend
		{
			last_allocated = new_block; //handle all last_allocated here
			return new_block + 1;
		}
	}
}

void my_free(void* ptr)
{
	if(ptr == NULL)
		return;

	// and here's where you free stuff.
	
	Header ptr_header = (Header)ptr - 1;  // find the header of ptr

	ptr_header->status = 1;  //mark the block free
	
	if (ptr_header == last_allocated)
		last_allocated = heap_head;
	//coalesce
	if (ptr_header->prev != NULL || ptr_header->next != NULL)
		ptr_header = coalesce(ptr_header);
  
	//contract
	contract(ptr_header);
}
