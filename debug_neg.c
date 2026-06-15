#include <stdio.h>
#include <string.h>
#include "include/ragesort.h"

int main() {
    int data[] = {0, -1, -2, -3, -4, -5};
    int n = 6;
    int expected[] = {-5, -4, -3, -2, -1, 0};
    
    printf("Original: ");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n");
    
    ragesort_auto(data, n);
    
    printf("Sorted:   ");
    for (int i = 0; i < n; i++) printf("%d ", data[i]);
    printf("\n");
    
    if (memcmp(data, expected, sizeof(data)) == 0) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
        printf("Expected: ");
        for (int i = 0; i < n; i++) printf("%d ", expected[i]);
        printf("\n");
    }
    
    return 0;
}