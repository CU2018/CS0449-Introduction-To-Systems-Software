/* Name: Alex Vallejo
 * Username: amv49
 * Peoplesoft: 357811
 * Date: 11/5/2013
 * CS 449 Project 3
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

void print_list();
void* split(int);
void* next_fit(int);

// Using this statement, we can refer to a Node as simply Node without the need
// to use pointers. It saves some typing.
typedef struct node *Node;

/*
 * ----------------------------------------------------------------------------
 * The node structure used to keep track of memory allocations in the heap.
 *  size => number of bytes this allocation contains
 *  free => 1 means true, 0 means false
 *  next => next node in the list. NULL if this node is the first or last node
 *  prev => previous node in the list. NULL if this node is the first node
 * ----------------------------------------------------------------------------
 */
struct node{
  unsigned int size;
  unsigned int free;
  struct node *next;
  struct node *prev;
};

// static global variables to keep track node locations pertinant to the linked
// list
static Node first = 0;
static Node cur = 0;
static Node last = 0;

/*
 * ----------------------------------------------------------------------------
 * Extend the heap when there is not enough space in the current heap for the
 * requested memory allocation.
 * ----------------------------------------------------------------------------
 */
Node extend_heap(unsigned int size){
  Node n;

  n = sbrk(0);
  if (sbrk(sizeof(struct node) + size) == (void*)-1)
    return NULL;

  n->size = size;
  n->free = 0;
  n->next = NULL;
  n->prev = last;

  last->next = n;
  last = n;

  return n;
}

/* ----------------------------------------------------------------------------
 * Run once when the list is first defined. Initializes the list.
 * --------------------------------------------------------------------------
 */
static Node init_list(unsigned int size){

  first = sbrk(size + sizeof(struct node));

  first->size = size;
  first->free = 0;
  first->next = NULL;
  first->prev = NULL;

  last = first;
  cur = first;

  return first;
}

/*
 * ----------------------------------------------------------------------------
 * Allocate space for a given dynamic memory request. The spaces are kept track
 * of by a linked list with nodes that store pertinant data RE: the allocation
 * ----------------------------------------------------------------------------
 */
void *my_next_fit_malloc(unsigned int size){

  // if this is the first call
  if (first == 0)
    return (void*)(init_list(size) + 1);

  else{
    Node n = next_fit(size);

    if (n == NULL)
      return (extend_heap(size) + 1);

    else{
      cur = n;
      return n;
    }
  }
}

/*
 * ----------------------------------------------------------------------------
 * Free space previously allocated by malloc. This memory can now be used again
 * Also, colless any contiguous free nodes into one large free node to make
 * larger spaces available for larger allocation requests
 * ----------------------------------------------------------------------------
 */
void my_free(void *ptr){

  // Mark the node as free
  Node n = (Node)ptr - 1;
  n->free = 1;

  // If contiguous free space nodes exist, combine them into a single node
  // AKA: coalesce!
  // First look and see if a node has extra space behind it
  if (n->prev != NULL){
    if (n->prev->free == 1){
      n->prev->size = n->size + sizeof(struct node);
      n->prev->next = n->next;

      if (last == n)
        last = n->prev;

      if (cur == n)
        cur = n->prev;

      n = n->prev;
    }
  }

  //Now look and see if a node in front of the node is free
  if (n->next != NULL){
    if (n->next->free == 1){
      n->size += n->next->size + sizeof(struct node);
      n->next = n->next->next;
    }
  }

  // When the last node is free, we can cut it off and resize the heap to
  // contain only wnat we need
  // AKA: truncating
  if (n == last){
    if (last == first){
      first = 0;
      cur = 0;
      last = 0;
      brk((void*)(n));
    }

    else if (last == cur){
      cur = cur->prev;
      last = last->prev;
      last->next = NULL;
      brk((void*)(n-1));
    }

    else{
      last = last->prev;
      last->next = NULL;
      brk((void*)(n-1));
    }
  }
}

/*
 * ----------------------------------------------------------------------------
 * Returns the next available location in the linked list based on the most
 * recent search position.
 * ----------------------------------------------------------------------------
 */
void* next_fit(unsigned int size){

  Node n = cur->next;

  while(n != NULL){

    // when the free space is exactl equal, avoid creating an extra node
    if (n->free == 1 && n->size == size){
      n->free = 0;
      cur = n;
      return n;
    }

    // when there is enough free space, adjust the space and create a new
    // node with the remaining free space
    if (n->free == 1 && n->size > (size + sizeof(struct node))){
      n = split(size);
      n->free = 0;
      n->size = size;
      return n;
    }

    printf(" n: %x", n);
    if (n->next == NULL){
      if (n == first){
        n = NULL;
      }

      else {
        n = first;
      }
    }

    else{
      if (n == cur)
        n = NULL;
      else
        n = n->next;
    }
  }

  return (void*)(extend_heap(size) + 1);
}

/*
 * ----------------------------------------------------------------------------
 * Split a large free space into space for the requested size and another free
 * space node representing the remaining free space in the initial free space
 * ----------------------------------------------------------------------------
 */
void* split(unsigned int size){

  Node n = cur + 1 + (size / sizeof(struct node));

  n->free = 0;
  n->next = cur->next;
  n->prev = cur;
  n->size = size;
  cur->next->prev = n;
  cur->next = n;

  return n;
}

/*
 * ----------------------------------------------------------------------------
 * Iterate through the malloc linked list and print each node and respective
 * values.
 * ----------------------------------------------------------------------------
 */
void print_list(){
  Node n = first;
  int i = 0;

  if (first != NULL){
    printf("\n\nBegin memory allocation region dump...");

    while(n != NULL){

      printf("\n");
      printf("\nNode: %d", i);
      printf("\nAddr: %d", n);
      printf("\nSize: %d", n->size);
      printf("\nFree: %d", n->free);
      printf("\nNext: %p", n->next);
      printf("\nPrev: %p", n->prev);

      n = n->next;
      i++;
    }// end while

    printf("\n");

  }// end if
}// end print_list: