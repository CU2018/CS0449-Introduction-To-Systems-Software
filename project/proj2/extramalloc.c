//siz24

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "mymalloc.h"

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

typedef struct header* Header;  //Header is defined as the pointer to the struct header
struct header   //20 bytes in 32 bit (-m32)
{	
	unsigned status; //0 for used and 1 for free
	unsigned data; //the size of the block
	struct header* prev;
	struct header* next;
	struct header* free;
};

//static global variables: head, tail, bins[3]
//only one heapcan be seen only in this file
static Header heap_head = NULL;
static Header heap_tail = NULL;
Header bins[3]; //0:small_bin (16-63), 1: medium_bin (64-127), 2: large_bin (>128)

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

	//set head
	heap_head->status = 0;  //mark it as used
	heap_head->data = size;
	heap_head->prev = NULL;
	heap_head->next = NULL;
	//set tail
	heap_tail = heap_head;
	//set bins
	//bins = {small_bin, medium_bin, large_bin};
	
	return heap_head;
}

int get_right_bin(unsigned int size)
{
	assert (size >= 16);
	if (size >=16 && size <=63)
		return 0;
	if (size >=64 && size <=127) 
		return 1;
	if (size > 128)
		return 2;
	return -1;
}

void insert_free(Header remained_free)
{
	int right_bin = get_right_bin(remained_free->data);
	if (right_bin != -1)
	{
		remained_free->free = bins[right_bin];//insert from the front, O(1)
		bins[right_bin] = remained_free;
	}
	
}

void remove_from_bins(Header remove_free)
{
	int right_bin = get_right_bin(remove_free->data);
	Header prev;
	if (right_bin != -1)
		prev = bins[right_bin];
	else 
		return;

	if (prev == remove_free) //remove the only block in bins or the first block
	{
		if (prev->free == NULL)
			bins[right_bin] = NULL;
		else 
			bins[right_bin] = bins[right_bin]->free;
	}
	else  
	{
		for (; prev != NULL; prev = prev->free)
		{
			if (prev->free == remove_free)  //find the prev free of remove_free
				prev->free = remove_free->free;
		}
	}
}

void split(unsigned int size, Header new_block)
{
	Header remained_free = (void*)new_block + size + sizeof(struct header); //count to the insert block
	
	if( new_block == heap_tail)   //check if the spliting block is the tail
		heap_tail = remained_free;
	
	remained_free->data = new_block->data - size - sizeof(struct header); //get the size of new free block
	remained_free->status = 1;  //mark as free
	
	//insert the remained_free header in the general list
	remained_free->prev = new_block;
	remained_free->next = new_block->next;
	new_block->next->prev = remained_free;
	new_block->next = remained_free;
	//insert the remained_free header in the right bin
	insert_free(remained_free);
	
	//update the old header
	new_block->status = 0;  //mark as used
	new_block->data = size;
}

Header find_in_bins(unsigned int size, Header this_bin)
{
	//all blocks in the bins have the status of 1 (free)
	Header new_block;
	for(new_block = this_bin; new_block != NULL; new_block = new_block->free)
	{
		if (size == new_block->data)  //no need to split
		{
			new_block->status = 0;  //mark as used
			return new_block;
		}
		if ((size + sizeof(struct header)) < (new_block->data)) //might need to split the block
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

Header quick_fit(unsigned int size)
{
	Header new_block = NULL;
	if (bins[0] != NULL && size >=16 && size <=63)
	{
		new_block = find_in_bins(size, bins[0]);
	}
	if (new_block == NULL && bins[1] != NULL && size >=64 && size <=127) //didn't find in small
	{
		new_block = find_in_bins(size, bins[1]);
	}
	if (new_block == NULL && bins[2] != NULL && size > 128)  //didn't find in small, medium
	{
		new_block = find_in_bins(size, bins[2]);
	}
	return new_block; //return NULL if there's no right free blocks in any bin		
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
	
	return new_block;
}

Header combine(Header block1, Header block2)
{
	assert(block1 != NULL && block2 != NULL);
	assert(block1->next == block2 && block2->prev == block1);
	
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
		remove_from_bins(free_header->prev);
		free_header = combine(free_header->prev, free_header);
	}
	if (free_header->next != NULL && free_header->next->status == 1) //has free next
	{
		remove_from_bins(free_header->next);
		free_header = combine(free_header, free_header->next);
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
			bins[0] = NULL;
			bins[1] = NULL;
			bins[2] = NULL;
			brk(free_header);  //move the break down to the free_header
		}
		else 
		{
			heap_tail = heap_tail->prev;
			heap_tail->next = NULL;
			remove_from_bins(free_header);
			brk(free_header);
		}
	}
}

void print_list()
{
	Header h = heap_head;
	int i = 0;

	if (h != NULL)
		printf("\nPrint the list...\n");
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
	printf("small_bin: %p\n", bins[0]);
	h = bins[0];
	i = 0;
	while(h != NULL)
	{
		printf("Header: %d", i);
		printf("  Addr: %p", h);
		printf("  Status: %d", h->status);
		printf("  Data: %d", h->data);
		printf("  Free: %p\n", h->free);
		h = h->free;
		i++;
	}
	i = 0;
	printf("medium_bin: %p\n", bins[1]);
	h = bins[1];
	while(h != NULL)
	{
		printf("Header: %d", i);
		printf("  Addr: %p", h);
		printf("  Status: %d", h->status);
		printf("  Data: %d", h->data);
		printf("  Free: %p\n", h->free);
		h = h->free;
		i++;
	}
	i = 0;
	printf("large_bin: %p\n", bins[2]);
	h = bins[2];
	while(h != NULL)
	{
		printf("Header: %d", i);
		printf("  Addr: %p", h);
		printf("  Status: %d", h->status);
		printf("  Data: %d", h->data);
		printf("  Free: %p\n", h->free);
		h = h->free;
		i++;
	}
	printf("Printing list end!\n\n");
	
}

void* quick_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;
	
	size = round_up_size(size);
	

	if (heap_head == NULL)	//the first block in the heap
		return (void*)(set_heap(size) + 1);  //return the data part(after the header), have to cast it into a pointer
	else //heap has blocks
	{
		Header new_block = quick_fit(size);  //find the next allocate block
		
		if (new_block == NULL)  //the next_fit not found, need extend
			return (expand_heap(size) + 1);   //return the data part(after the header)
		else //find a block to reuse, no need to extend
		{
			remove_from_bins(new_block); 
			return new_block + 1;
		}
	}
}

void quick_free(void* ptr)
{
	if(ptr == NULL)
		return;

	// and here's where you free stuff.
	
	Header ptr_header = (Header)ptr - 1;  // find the header of ptr

	ptr_header->status = 1;  //mark the block free
	
	//coalesce
	if (ptr_header->prev != NULL || ptr_header->next != NULL)
		ptr_header = coalesce(ptr_header);
	insert_free(ptr_header);  //insert the new free block into the right bin
  
	//contract
	contract(ptr_header);
}
