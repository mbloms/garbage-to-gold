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

void collect(void *roots[]){//The big bad function, kallar på hjälpmetoder

}

void memscan(memheader heado){ //Tar ett block, letar efter adresser till andra block.
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

void memcopy(){


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
