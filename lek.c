#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct header {
    void * next;
    unsigned int size;
    void * forwarding;
} memheader;

void* heap;
static memheader* heappointer;


// gc'd allocation
void * galloc(int size){
    heappointer->next = (void*)heappointer + sizeof(memheader) + size;
    heappointer->size=size;
    printf("heappointer: %p\nnext: %p\nsizeof(memheader): %d\nsize: %d\n",heappointer, heappointer->next, sizeof(memheader), size);
    heappointer->forwarding = NULL;
    void* new_block = heappointer + sizeof(memheader);
    heappointer = heappointer->next;
    return new_block;
}

void collect(void *roots[]){

}

int main(int argc, char *argv[]) {
    printf("Hello World\n");

    heap = malloc(1024*1024*1024);
    heappointer = (memheader*) &heap;
    char* hej;

    printf("%d\n", sizeof(memheader));

    while(1) {
        hej = galloc(20);
        strcpy(hej,"hej");
        printf("%s\n%p\n", hej,hej);
    }



    //low = malloc(1024*1024*1024);
    //empty = malloc(1024*1024*1024);
    //printf("Skriv något\n> ");
    //scanf("%s", high);
    //printf("%s\n",high);
    exit(0);
  }
