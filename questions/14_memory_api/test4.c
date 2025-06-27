#include <stdio.h>
#include <stdlib.h>

int main() {
    int* array = (int*)malloc(10 * sizeof(int));
    array[0] = 42;
    printf("Value: %d\n", array[0]);
    return 0;
}
