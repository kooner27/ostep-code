#include <stdlib.h>
#include <stdio.h>

int main() {
    // int* pointer = (int *) malloc(sizeof(int));
    // *pointer = 5;
    // printf("%d\n", *pointer);
    int* pointer = (int *) malloc(sizeof(int));
    printf("%d\n", *pointer);
    pointer = NULL;
    printf("%d\n", *pointer);
    return 0;
}
