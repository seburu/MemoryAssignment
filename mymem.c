#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList{
    // doubly-linked list
    struct memoryList *prev;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // f
    void *ptr;           // location of block in memory pool.
};


strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;


static struct memoryList *head;
static struct memoryList *nextFit;
//struct memoryList* prevNode = NULL;

//Prototype
struct memoryList * first(size_t size);
struct memoryList * worst(size_t size);
struct memoryList * next(size_t size);
struct memoryList * best(size_t size);


struct memoryList *first(size_t size){
    struct memoryList *current = head;

    while(current!=NULL){
      if(current->size >= size && current->alloc==0){
          break;
      }
      current = current->next;
    }
    //printf("No free space was big enough for this memoryBlock");
    return current; //Can be NULL
}

struct memoryList *next(size_t size){
    if(nextFit == NULL){
        nextFit = head;
    }
    struct memoryList *current = nextFit, *roundtrip = nextFit;

    do{
        if(current->size >= size && current->alloc==0){
            break;
        }
        //Treat as circular list
        if(current->next == NULL){ current = head;}
        else { current = current->next; }

    } while (current != roundtrip);
    //printf("No free space was big enough for this memoryBlock");

    if(current->alloc == 0 && current->size >= size){
        return current;
    }
    return NULL; //Can be NULL
}

void *insertMemBlock(struct memoryList* current, size_t size){
    if(current == NULL){return NULL;}
    if(current->size == size){ current->alloc = 1; return current->ptr;}

    //assumption, node size is bigger than 'size'

    struct memoryList *extraNode = malloc(sizeof(struct memoryList));
// fix pointers (prev, next) TODO
    //current next ikke findes check
    extraNode->prev = current;
    extraNode->next = current->next;
    if(current->next != NULL){
        current->next->prev = extraNode;
    }
    current->next = extraNode;

// fix data (size, alloc, ptr) TODO
    extraNode->size = current->size - size;
    current->size = size;
    current->alloc = 1;
    extraNode->alloc = 0;
//  current->ptr = current->ptr;
    extraNode->ptr = current->ptr + size;
    nextFit = current;
    return current->ptr;
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

void initmem(strategies strategy, size_t sz){
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    //Husk at iterere igennem listen!!
    if(head!=NULL){
        struct memoryList *tmp = head;
        while(head != NULL){
            head = head->next;
            free(tmp);
            tmp = tmp->next;
        }
    }

    myMemory = malloc(sz);
    /* TODO: Initialize memory management structure. */
    head = malloc(sz);
    head->next = NULL;
    head->prev = NULL;
    head->alloc = 0;
    head->size = sz;
    head->ptr = myMemory;

}
struct memoryList *best(size_t requested){
    struct memoryList *current = head, *choice = NULL;
    //brug firstFit her i stedet. Midlertidig l??sning:
    int diff = mySize + 1;
    while(current != NULL){
        if(current->alloc == 0
           && current->size < diff
           && current->size >= requested){
            diff = current->size;
            choice = current;

    }
    current = current->next;
}
//return best fit node
return choice;
}

struct memoryList *worst(size_t requested){
    struct memoryList *current = head, *choice = NULL;
    //brug firstFit her i stedet. Midlertidig l??sning:
    int diff = 0;
    while(current != NULL){
        if(current->alloc == 0
           && current->size > diff
           && current->size >= requested){
            diff = current->size;
            choice = current;

        }
        current = current->next;
    }
//return best fit node
    return choice;
}


/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */


void *mymalloc(size_t requested){
    struct memoryList *current;
    assert((int)myStrategy > 0);

    switch (myStrategy)
    {
        case NotSet:
            return NULL;
        case First:
            current = first(requested);
            break;
        case Best:
            current = best(requested);
            break;
        case Worst:
            current = worst(requested);
            break;
        case Next:
            current = next(requested);
            break;
    }

    //myMalloc on current with requested size.

    //return noget!
    return insertMemBlock(current,requested);
}

// 1 til 0. Hvis der er naboer der er 0 merges de.
/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block){

    struct memoryList* current = head, *toBeDeleted;
    if(current == NULL){return;}
    while (current != NULL){
        if(current->ptr == block){
            break;
        }
        current = current->next;
    }
    if(current == NULL){return;}

    current->alloc = 0;
    //check if prev node exist and is also null, then merge
    if(current->prev != NULL && current->prev->alloc == 0){
        //set struct pointers to look at what they're supposed to be used
        toBeDeleted = current;
        current = current->prev;

        //fix data
        current->size += toBeDeleted->size;
        //current->alloc = 0;

        //fix pointers
        current->next = toBeDeleted->next;
        if(toBeDeleted->next != NULL){
            toBeDeleted->next->prev = current;
        }

        free(toBeDeleted);
    }

    //check if next node exist and is also null, then merge
    if(current->next != NULL && current->next->alloc == 0){
        //set struct pointers to look at what they're supposed to be used
        toBeDeleted = current->next;
        //current = current;

        //fix data
        current->size += toBeDeleted->size;
        //current->alloc = 0;

        //fix pointers
        current->next = toBeDeleted->next;
        if(toBeDeleted->next != NULL){
            toBeDeleted->next->prev = current;
        }

        free(toBeDeleted);
    }
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */
// Skal vi burge funktionerne til vores strategier?
// hvad menes med den her?
/* Get the number of contiguous areas of free space in memory. */
int mem_holes(){
    int counter = 0;
    struct memoryList *current = head;
    while(current != NULL){
        if(current->alloc == 0){
            counter++;
        }
        current = current->next;
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
        if(current->alloc == 0 && current->size > large){
            large = current->size;
        }
        current = current->next;
    }
    return large;
}

/* Number of free blocks smaller than or equal to "size" bytes. */
int mem_small_free(int size){
    struct memoryList *current = head;
    int smallnodes = 0;
    while(current!=NULL){
        if(current->alloc == 0 && current->size<=size){
            smallnodes++;
        }
        current = current->next;
    }
    return smallnodes;
}

//Is a particular byte allocated or not?
char mem_is_alloc(void *ptr){
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
void *mem_pool(){
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
    /*
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    A simple example.
       Each algorithm should produce a different layout.

    initmem(strat,500);
    //my malloc skal returnerer en pointer til hvor der er blevet indsat noget?
    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();
*/
}
