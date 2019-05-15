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
    top_block->next = top_block + sizeof(memheader) + top_block->size;
    return stack;
}

void copy_block(memheader* block) {
    memcpy(block+1, block->forwarding, block->size);
}

memheader* scan_block(memheader* block, memheader* stack) {

}


void collect(memheader* block) {
    //The big bad function, kallar på hjälpmetoder
    memheader* scan_stack = NULL;
    memheader* copy_stack = NULL;



    push_block(scan_stack, block);

    while (scan_stack != NULL) {
        //Reusing "block" instead of adding new temp variable
        block = scan_stack;
        scan_stack = pop_block(scan_stack);
        scan_stack = scan_block(block,scan_stack);
        push_block(copy_stack, block);
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

void memscan(memheader heado){
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


}

int main(int argc, char *argv[]) {

    char* hej;
    int i = 1;
    for(char* hej = galloc(1024*8); hej!=NULL; hej = galloc(1024*8)) {
        sprintf(hej,"hej %d", i);
        //printf("Text: %s\nAmount of Hejs: %d\n", hej, i);
        i++;
    }

    for(memheader* p = heap.start;
        ptrdiff(p, heap.start) < MAX_HEAPSIZE;
        p=p->next) {
        printf("%p\n%s\n\n", p, p+1);
    }



    //low = malloc(1024*1024*1024);
    //empty = malloc(1024*1024*1024);
    //printf("Skriv något\n> ");
    //scanf("%s", high);
    //printf("%s\n",high);
    exit(0);
  }
