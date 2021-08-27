#include <stdio.h>
#include <stdlib.h>

#define NUM_INTS 100
//This file will run a math problem and diagnostics.
//output will be to println.
//BASH script that calls this can compile output and send over Network.

int main() {
    //print out the memory space.
    int * space = (int *)malloc(sizeof(int)*NUM_INTS); // Variable to store value from analog reads
    if(space == NULL) {
      printf("mallocfailed");
    }
    for(int i = 0; i<NUM_INTS; i++){
      space[i] = 2147483647;
    }
    for(int i = 0; i<NUM_INTS; i++){
      printf("%X", space[i]);
    }
    return 0;
}
