#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int* data = (int*)malloc(100 * sizeof(int));
    // we allocated index 0 to 99
    free(data);
    printf("value of data[0]: %d\n", data[0]);

    return 0;
}
