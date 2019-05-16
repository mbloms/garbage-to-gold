#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct header {
    struct header * next;
    unsigned int size;
    void * forwarding;
} memheader;

#define MAX_HEAPSIZE (1024*1024*1024)

typedef struct gc_heap {
    void* start;
    void* end;
    void* gbreak;
} gc_heap;

gc_heap heap;
gc_heap empty;

unsigned int ptrdiff(void* a, void* b) {
    return ((char *)a) - ((char *)b);
}


// gc'd allocation
void * galloc(int size){
    if (heap.end == NULL) {

        heap.start = malloc(MAX_HEAPSIZE);
        heap.gbreak = heap.start;
        heap.end = (char*) (heap.start) + MAX_HEAPSIZE;
    }

    if ((ptrdiff(heap.gbreak, heap.start) + sizeof(memheader) + size) > MAX_HEAPSIZE) {
        printf("ajabaja\n");
        //collect();
        return NULL;
    }

    memheader* header = heap.gbreak;
    void* block = heap.gbreak + sizeof(memheader);
    heap.gbreak += sizeof(memheader) + size;
    //printf("Header: %p\nBlock: %p\nHeapened: %p\n", header, block, heapend);

    header->next = heap.gbreak;
    header->size = size;
    header->forwarding = NULL;

    return block;
}

/* Push block onto stack and return block (new stack)
*/
memheader* push_block(memheader* stack, memheader* block) {
    block->next = stack;
    return block;
}

memheader* pop_block(memheader* stack) {
    memheader* top_block = stack;
    stack = stack->next;
    //OBS!! KAN BLI FEL
    top_block->next = top_block + sizeof(memheader) + top_block->size;
    if (ptrdiff(top_block->next, top_block+1) != top_block->size) {
        fprintf(stderr, "\nWARNING: top_block->next incorrect\n");
    }
    return stack;
}

/* returns a pointer to the header of the
   forwarded or forwarding block
*/
memheader* forw_header(memheader* stack) {
    return (memheader*) stack->forwarding -1;
}

/* Takes a pointer to the new block
 * and copies the old data to the new location
*/
void copy_block(memheader* block) {
    memcpy(block+1, block->forwarding, block->size);
    block->forwarding = NULL;
}

memheader* balloc(memheader* old_block) {
    memheader* new_block = galloc(old_block->size);
    old_block->forwarding = new_block+1;
    new_block->forwarding = old_block+1;
    return new_block;
}


memheader* scan_block(memheader* block, memheader* scan_stack) {
    printf("\nblock: %p\n", block);
    memheader** scanner = (memheader**) (block+1);
    while((memheader*) scanner < block->next) {
        printf("\nscanner: %p\n", scanner);
        printf("\nheap.start: %p \n*scanner: %p \nheap.end: %p\n", heap.start, *scanner, heap.end);

        if ((memheader*) empty.start < *scanner) {
            printf("hej\n");
            if (*scanner < (memheader*) empty.end) {
                printf("du\n");
                memheader* found = (*scanner)-1;
                printf("\nfound: %p\n", found);
                if (found->forwarding == NULL) {
                    printf("wow\n");
                    scan_stack = push_block(scan_stack, found);
                    memheader* new_block = balloc(found);
                    *scanner = found->forwarding;
                }
            }
        }
        scanner++;
    }
    return scan_stack;
}


void collect(memheader* block) {
    //The big bad function, kallar på hjälpmetoder
    memheader* scan_stack = NULL;
    memheader* copy_stack = NULL;

    gc_heap tmp = heap;
    heap = empty;
    empty = tmp;

    //scan_block will not balloc the initial block, only found ones.
    push_block(scan_stack, balloc(block));

    while (scan_stack != NULL) {
        //Reusing "block" instead of adding new temp variable

        block = forw_header(scan_stack);
        scan_stack = pop_block(scan_stack);

        scan_stack = scan_block(block,scan_stack);

        push_block(copy_stack, forw_header(block));
    }

    while (copy_stack != NULL) {
        //Reusing "block" instead of adding new temp variable
        block = copy_stack;
        copy_stack = pop_block(copy_stack);

        copy_block(block);
    }
}


//void collect(void *roots[]){

//}

//void memscan(memheader heado){
    //Tar ett block, letar efter adresser till andra block.
    //Allokerar nytt minne på nya heapen för att täcka sizeof current Block
    //Antingen galloc eller annan hjälpmetod
    //Varje gång vi hittar adress, allokera nytt block för den
    //Har referens till senaste blocket den allokerade.
    //har den inte allokerat något, dags att kopiera data, kalla på memcopy()

    //////////////////////////////////////////////
    ////////////  Deep, Dual-Stack   /////////////
    //////////////////////////////////////////////




    //////////////////////////////////////////////
    ///////////     Deep, Recursive     //////////
    //////////////////////////////////////////////
    /*galloc(heado);
    while(nextmemory != NULL){
        if(addressisFound()){
            TheList.addToList(address);
            ListCounter++;
        }
    }
    for(address ad : theList){
        memscan(ad);
        ListCounter--;
    }
    if(ListCounter == 0){
        memcopy(this);
    }*/


//}

int main(int argc, char *argv[]) {

    void* data;
    void** p;

    data = galloc(sizeof(void*));

    for (size_t i = 0; i < 20; i++) {
        p = galloc(sizeof(void*));
        *p = data;
        data = p;
        printf("%p\n", *p);
    }

    memheader* block = *p;
    block--;

    printf("p: %p\n", p);
    printf("data: %p\n", *p);
    printf("**p: %p\n", ** (void***) p);
    printf("block: %p\n", block);

    collect(block);



    //low = malloc(1024*1024*1024);
    //empty = malloc(1024*1024*1024);
    //printf("Skriv något\n> ");
    //scanf("%s", high);
    //printf("%s\n",high);
    exit(0);
  }
