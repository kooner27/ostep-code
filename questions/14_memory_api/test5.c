#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int* data = (int*)malloc(100 * sizeof(int));
    // we allocated index 0 to 99
    data[100] = 0;
    printf("value of data[100]: %d\n", data[100]);

    return 0;
}
