#include <stdio.h>

#define NUM_INTS 5000
//This file will run a math problem and diagnostics.
//output will be to println.
//BASH script that calls this can compile output and send over Network.

int main() {
    //print out the memory space.
    space = (int *)malloc(sizeof(int)*NUM_INTS); // Variable to store value from analog reads
    if(space == NULL) {
      print("mallocfailed");
    }
    for(int i = 0; i<NUM_INTS; i++){
      space[i] = 2147483647;
    }
    for(int i = 0; i<NUM_INTS; i++){
      print("%X", space[i]);
    }
    return 0;
}
