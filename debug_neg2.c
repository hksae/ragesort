#include <stdio.h>
#include <string.h>
#include "include/ragesort.h"

int main() {
    int data[] = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9};
    int n = 10;
    
    printf("Original: ");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n");
    
    int* out = malloc(n * sizeof(int));
    int* work = malloc(n * sizeof(int));
    
    if (!out || !work) {
        printf("malloc failed\n");
        return 1;
    }
    
    // Заполняем out мусором
    memset(out, 0xAA, n * sizeof(int));
    
    ragesort(data, n, out, work, RAGESORT_DEFAULT);
    
    printf("Data after sort: ");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n");
    
    printf("Out buffer:      ");
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("\n");
    
    free(out);
    free(work);
    
    return 0;
}