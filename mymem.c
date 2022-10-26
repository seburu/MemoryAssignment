#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
    // doubly-linked list
    struct memoryList *prev;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
                         // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};


strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;


static struct memoryList *head;
static struct memoryList *nextFit;
//struct memoryList* prevNode = NULL;

struct memoryList *first(size_t size){
    struct memoryList *current = head;
    struct memoryList *first;

    while(current!=NULL){
      if(current->size > size && current->alloc==0){
          first = current;
      }
      current = current->next;
    }
    //printf("No free space was big enough for this memoryBlock");
    return first;
}

void insertMemBlock(struct memoryList* current, size_t size){
    int newSize = current->size - size;
    struct memoryList *reqNode;
    struct memoryList *extraNode;

    extraNode = current;
    reqNode->size = size;
    extraNode->size = newSize;

    extraNode->prev->next = reqNode;
    reqNode->prev = extraNode->prev;
    extraNode->prev = reqNode;
    reqNode->next = extraNode;
    reqNode->ptr = &reqNode;
}


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    if(head!=NULL){
        free(head);
    }


    myMemory = malloc(sz);
    printf(myMemory);
    /* TODO: Initialize memory management structure. */
    head->next = NULL;
    head->prev = NULL;
    head->alloc = 0;
    head->size = sz;
    head->ptr = myMemory;


}

struct memoryList bestFit(size_t requested){
    struct memoryList current = *head;
    //brug firstFit her i stedet. Midlertidig løsning:
    struct memoryList best = *first(requested);

    while(current.next != NULL){
        if(current.alloc == 0){
            int diff = current.size - requested;
            if (diff >= 0 && diff < (best.size - requested)){
                best = current;
            }
        }
        current = *current.next;
    }
    //return best fit node
    return best;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */


void *mymalloc(size_t requested)
{
    struct memoryList *current;
    assert((int)myStrategy > 0);

    switch (myStrategy)
    {
        case NotSet:
            return NULL;
        case First:
            return NULL;
        case Best:
            *current = bestFit(requested);
        case Worst:
            return NULL;
        case Next:
            return NULL;
    }

    //myMalloc on current with requested size.
    insertMemBlock(current,requested);

}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    if(block != NULL){
        free(block);
    }
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    int counter = 0;
    struct memoryList current = *head;
    struct memoryList next;
    while(current.next != NULL){
        next = *current.next;
        if(next.alloc == '0' && current.alloc == '0'){
            counter++;
        }
        current = *current.next;
    }
    return counter;
}

/* Get the number of bytes allocated */
int mem_allocated(){
    struct memoryList *current = head;
    int allo = 0;
    while(current!=NULL){
        if(current->alloc == 1){
            allo = allo+current->size;
        }
        current = current->next;
    }
    return allo;
}

/* Number of non-allocated bytes */
int mem_free(){
    struct memoryList *current = head;
    int free = 0;
    while(current!=NULL){
        if(current->alloc == 0){
            free = free+current->size;
        }
        current = current->next;
    }
    return free;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free(){
    struct memoryList *current = head;
    int large = 0;
    while(current!=NULL){
        if(current->alloc == 1 && current->size>large){
            large = current->size;
        }
        current = current->next;
    }
    return large;
}

/* Number of free blocks smaller than or equal to "size" bytes. */
int mem_small_free(int size)
{
    struct memoryList *current = head;
    int smallnodes = 0;
    while(current!=NULL){
        if(current->alloc == 0 && current->size<size){
            smallnodes++;
        }
        current = current->next;
    }
    return smallnodes;
}

//Is a particular byte allocated or not?
char mem_is_alloc(void *ptr)
{
    struct memoryList *current = head;
    while(current!=NULL){
        if(current->ptr == ptr && current->alloc==1){
           return 1;
        }
        current = current->next;
    }
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    return;
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat,500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();

}
