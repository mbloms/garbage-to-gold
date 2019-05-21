#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct header {
    struct header * next;
    unsigned int size;
    int security_level;
    void * forwarding;
} memheader;

#define MAX_HEAPSIZE (1024*1024*1024)

#define LOW 0
#define HIGH 1

typedef struct gc_heap {
    void* start;
    void* end;
    void* gbreak;
    int security_level;
} gc_heap;

gc_heap highHeap = {NULL, NULL, NULL, HIGH};
gc_heap lowHeap = {NULL, NULL, NULL, LOW};
gc_heap empty = {NULL, NULL, NULL, -1};

gc_heap* current_heap;
gc_heap* old_heap;

//////////////////////////////////////////////
///////////////    ptrdiff    ////////////////
//////////////////////////////////////////////
unsigned int ptrdiff(void* a, void* b) {
    return ((char *)a) - ((char *)b);
}

//////////////////////////////////////////////
////////////////    galloc    ////////////////
//////////////////////////////////////////////
// gc'd allocation
void * galloc(int size){
    if (current_heap->end == NULL) {

        current_heap->start = malloc(MAX_HEAPSIZE);
        current_heap->gbreak = current_heap->start;
        current_heap->end = (char*) (current_heap->start) + MAX_HEAPSIZE;
    }

    if ((ptrdiff(current_heap->gbreak, current_heap->start) + sizeof(memheader) + size) > MAX_HEAPSIZE) {
        printf("ajabaja\n");
        //collect();
        return NULL;
    }

    memheader* header = current_heap->gbreak;
    void* block = current_heap->gbreak + sizeof(memheader);
    current_heap->gbreak += sizeof(memheader) + size;
    //printf("Header: %p\nBlock: %p\nHeapened: %p\n", header, block, heapend);

    header->next = current_heap->gbreak;
    header->size = size;
    header->security_level = current_heap->security_level;
    header->forwarding = NULL;

    return block;
}

//////////////////////////////////////////////
///////////////   push_block   ///////////////
//////////////////////////////////////////////
/* Push block onto stack and return block (new stack)
*/
memheader* push_block(memheader* stack, memheader* block) {
    block->next = stack;
    return block;
}

//////////////////////////////////////////////
///////////////    pop_block   ///////////////
//////////////////////////////////////////////
memheader* pop_block(memheader* stack) {
    memheader* top_block = stack;
    stack = stack->next;
    //OBS!! KAN BLI FEL
    top_block->next = ((void*) top_block) + sizeof(memheader) + top_block->size;
    if (ptrdiff(top_block->next, top_block+1) != top_block->size) {
        fprintf(stderr, "\nWARNING: top_block->next incorrect\n");
    }
    return stack;
}

//////////////////////////////////////////////
//////////////   forw_header   ///////////////
//////////////////////////////////////////////
/* returns a pointer to the header of the
   forwarded or forwarding block
*/
memheader* forw_header(memheader* stack) {
    return ((memheader*) (stack->forwarding)) -1;
}

//////////////////////////////////////////////
///////////////   copy_block   ///////////////
//////////////////////////////////////////////
/* Takes a pointer to the new block
 * and copies the old data to the new location
*/
void copy_block(memheader* old_block) {
    void* old_data = old_block+1;
    memheader* new_block = forw_header(old_block);
    void* new_data = old_block->forwarding;

    fprintf(stderr, "moving\n%p ->\t%p\n",old_block, new_block);

    if (forw_header(new_block) != old_block) {
        fprintf(stderr, "WARNING: forw_header(new_block) != old_block\nnew: %p\nold: %p\n", new_block, old_block);
    }

    memcpy(new_data, old_data, old_block->size);

    fprintf(stderr, "moving complete!\n\n" );
}

memheader* balloc(memheader* old_block) {
    memheader* new_data = galloc(old_block->size);
    memheader* new_block = new_data-1;
    old_block->forwarding = new_data;
    new_block->forwarding = old_block+1;
    new_block->security_level = old_block->security_level;
    return new_block;
}

bool addr_in_heap(void* address) {
    if (highHeap.start < address) {
        if (address < highHeap.end) {
            return true;
        }
    }
    if (lowHeap.start < address) {
        if (address < lowHeap.end) {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////
///////////////   scan_block   ///////////////
//////////////////////////////////////////////
memheader* scan_block(memheader* block, memheader* scan_stack) {
    fprintf(stderr, "scanning block:\t\t%p\n", block);
    memheader** scanner = (memheader**) (block+1);
    while((memheader*) scanner < block->next) {
        if ((memheader*) old_heap->start < *scanner) {
            if (*scanner < (memheader*) old_heap->end) {
                fprintf(stderr, "found address:\t\t%p\n", *scanner);
                memheader* found = (*scanner)-1;
                if (found->forwarding == NULL) {
                    scan_stack = push_block(scan_stack, balloc(found));
                    fprintf(stderr,"balloc:\t\t\t%p\n", scan_stack);
                } else if (forw_header(forw_header(found)) == found) {
                    fprintf(stderr, "address already forwarded");
                } else {
                    scanner++;
                    continue;
                }
                fprintf(stderr,"forwarding:\t\t%p\n", found->forwarding);
                fprintf(stderr,"current scanner:\t%p\n", *scanner);
                *scanner = found->forwarding;
                fprintf(stderr,"updated scanner:\t%p\n", *scanner);
            }
        }
        scanner++;
    }
    fprintf(stderr, "scanning complete!\n\n", block);
    return scan_stack;
}

/////////////////////////////////////////////
///////////////   Collect   /////////////////
/////////////////////////////////////////////
void collect(int rootlen, void** roots) {

    memheader *scan_stack, *block;
    scan_stack = NULL;

    empty.security_level = current_heap->security_level;

    gc_heap tmp = *current_heap;
    *current_heap = empty;
    empty = tmp;

    old_heap = &empty;

    for (int i = 0; i < rootlen; i++) {
        if (roots[i] != NULL) {
            block = roots[i];
            //The header is located before the data.
            block--;
            //scan_block will not balloc the initial block, only found ones.
            //push a newly allocated block on the stack:
            scan_stack = push_block(scan_stack, balloc(block));
        }
    }

    while (scan_stack != NULL) {

        //forw_header gives back the old block
        block = forw_header(scan_stack);
        scan_stack = pop_block(scan_stack);

        scan_stack = scan_block(block,scan_stack);

        copy_block(block);
    }

    //reset all forwarding pointers
    block = current_heap->start;
    while (block < (memheader*) current_heap->gbreak) {
        block->forwarding = NULL;
        block = block->next;
    }

    //give back the new roots
    for (int i = 0; i < rootlen; i++) {
        if (roots[i] != NULL) {
            block = roots[i];
            block--;
            roots[i] = block->forwarding;
        }
    }

    //reset the empty heap
    empty.gbreak = empty.start;
    empty.security_level = -1;

}


/////////////////////////////////////////////
///////////////     Main    /////////////////
/////////////////////////////////////////////

#define SIZE1 100000000
#define SIZE2 10000

int main(int argc, char *argv[]) {

    int h = 2;

    sscanf(argv[1], "%d\n", &h);

    int* a = galloc(SIZE1);
    int* b = NULL;
    int* c = NULL;
    int* d = NULL;

    if (h > 10) {
        b = galloc(SIZE1);
        d = a;
    }
    else {
        c = galloc(SIZE1);
        b = a;
    }
    c = NULL;
    clock_t before, after;
    before = clock();
    int* x = galloc(SIZE2);

    void* roots[] = {a,b,c,d,x};
    collect(5, roots);
    after = clock();
    long diff = after - before;

    printf("cycles: %d\n", diff);
  }
