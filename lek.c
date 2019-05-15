#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct header {
    void * next;
    unsigned int size;
    void * forwarding;
} memheader;

void* heapstart;
void* heapend;


// gc'd allocation
void * galloc(int size){
    if (heapend == NULL) {
        heapstart = malloc(1024*1024*1024);
        heapend = &heapstart;
    }

    memheader* header = heapend;
    void* block = heapend + sizeof(memheader);
    heapend += sizeof(memheader) + size;
    printf("%p\n%p\n%p\n", header, block, heapend);

    header->next = heapend;
    header->size = size;
    header->forwarding = NULL;

    return block;
}

void collect(void *roots[]){

}

int main(int argc, char *argv[]) {
    printf("Hello World\n");

    printf("%p\n%p\n", heapend, heapstart);
    char* hej;

    while(1) {
        hej = galloc(20);
        strcpy(hej,"hej");
        printf("%s\n", hej);
    }



    //low = malloc(1024*1024*1024);
    //empty = malloc(1024*1024*1024);
    //printf("Skriv något\n> ");
    //scanf("%s", high);
    //printf("%s\n",high);
    exit(0);
  }
