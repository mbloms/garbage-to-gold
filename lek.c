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
    top_block->next = ((void*) top_block) + sizeof(memheader) + top_block->size;
    if (ptrdiff(top_block->next, top_block+1) != top_block->size) {
        fprintf(stderr, "\nWARNING: top_block->next incorrect\n");
    }
    return stack;
}

/* returns a pointer to the header of the
   forwarded or forwarding block
*/
memheader* forw_header(memheader* stack) {
    return ((memheader*) (stack->forwarding)) -1;
}

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

    new_block->forwarding = NULL;

    fprintf(stderr, "moving complete!\n\n" );
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
    //push a newly allocated block on the stack:
    scan_stack = push_block(scan_stack, balloc(block));

    while (scan_stack != NULL) {
        //Reusing "block" instead of adding new temp variable

        //forw_header gives back the old block
        block = forw_header(scan_stack);
        scan_stack = pop_block(scan_stack);

        scan_stack = scan_block(block,scan_stack);

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

    struct List {
        char* head;
        struct List * tail;
    };

    struct List * list = galloc(sizeof(struct List));
    list->tail = galloc(sizeof(struct List));
    list->tail->tail = galloc(sizeof(struct List));
    list->tail->tail->tail = galloc(sizeof(struct List));
    list->tail->tail->tail->tail = galloc(sizeof(struct List));
    list->tail->tail->tail->tail->tail = NULL;

    char* str = galloc(20);
    strcpy(str, "hej");
    list->head = str;

    str = galloc(20);
    strcpy(str, "du");
    list->tail->head = str;

    str = galloc(20);
    strcpy(str, "ditt");
    list->tail->tail->head = str;

    str = galloc(20);
    strcpy(str, "lilla");
    list->tail->tail->tail->head = str;

    str = galloc(20);
    strcpy(str, "skräp");
    list->tail->tail->tail->tail->head = str;

    memheader* block = (void*) list->tail->tail->tail;
    block--;
    collect(block);

    while(list!=NULL) {
        printf("%p\t%p\t%s\n", list, list->head, list->head);
        list=list->tail;
    }

    printf("%s\n\n" );

    list=block->forwarding;

    while(list!=NULL) {
        printf("%p\t%p\t%s\n", list, list->head, list->head);
        list=list->tail;
    }
  }
