#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    printf("Hello World\n");
    char *high = malloc(1024*1024*1024);
    char *low = malloc(1024*1024*1024);
    char *free = malloc(1024*1024*1024);
    printf("Skriv något\n> ");
    scanf("%s", high);
    printf("%s\n",high);
    printf("Skriv mer!\n> ");
    scanf("%s", low);
    printf("%s\n",low);
    printf("MEEER!!!\n> ");
    scanf("%s", free);
    printf("%s\n",free);
    scanf("%s", NULL);
    exit(0);
  }

// gc'd allocation
void * galloc(int size){
    
}
