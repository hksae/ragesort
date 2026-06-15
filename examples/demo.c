/**
 * @file demo.c
 * @brief Demonstration of RageSort usage.
 */

#include "../include/ragesort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_array(const char* title, int* arr, int n) {
    printf("%s: ", title);
    for (int i = 0; i < n && i < 20; i++) {
        printf("%d ", arr[i]);
    }
    if (n > 20) printf("...");
    printf("\n");
}

void demo_ragesort_auto() {
    printf("\n=== Demo 1: ragesort_auto (automatic memory) ===\n");
    
    int data[] = {5, -3, 42, -17, 0, 100, -42, 7, -100, 23};
    int n = sizeof(data) / sizeof(data[0]);
    
    print_array("Original", data, n);
    ragesort_auto(data, n);
    print_array("Sorted", data, n);
}

void demo_ragesort_manual_inplace() {
    printf("\n=== Demo 2: ragesort manual with RAGESORT_INPLACE ===\n");
    
    int data[] = {100, -50, 25, -75, 0, 50, -25, 75, -100, 10};
    int out[10];
    int work[10];
    int n = sizeof(data) / sizeof(data[0]);
    
    printf("Original data: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    printf("Out buffer (before): ");
    for (int i = 0; i < n; i++) {
        out[i] = 0xDEADBEEF;
        printf("0x%X ", out[i]);
    }
    printf("\n");
    
    ragesort(data, n, out, work, RAGESORT_INPLACE);
    
    printf("Data (unchanged): ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    printf("Out buffer (sorted): ");
    for (int i = 0; i < n; i++) {
        printf("%d ", out[i]);
    }
    printf("\n");
}

void demo_ragesort_manual_default() {
    printf("\n=== Demo 3: ragesort manual with RAGESORT_DEFAULT ===\n");
    
    int data[] = {50, -30, 20, -10, 0, 40, -20, 30, -40, 10};
    int out[10];
    int work[10];
    int n = sizeof(data) / sizeof(data[0]);
    
    print_array("Original data", data, n);
    print_array("Out buffer (before)", out, n);
    
    ragesort(data, n, out, work, RAGESORT_DEFAULT);
    
    print_array("Data (after copy)", data, n);
    print_array("Out buffer", out, n);
}

void demo_large_array() {
    printf("\n=== Demo 4: Large array (100000 elements) ===\n");
    
    const int n = 100000;
    int* data = (int*)malloc(n * sizeof(int));
    
    if (!data) {
        printf("Failed to allocate memory\n");
        return;
    }
    
    srand(42);
    for (int i = 0; i < n; i++) {
        data[i] = (rand() % 2000000) - 1000000;
    }
    
    printf("Generated %d random integers\n", n);
    printf("First 10 elements: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    ragesort_auto(data, n);
    
    printf("First 10 elements after sort: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    int sorted = 1;
    for (int i = 1; i < n; i++) {
        if (data[i - 1] > data[i]) {
            sorted = 0;
            break;
        }
    }
    printf("Verification: %s\n", sorted ? "PASSED" : "FAILED");
    
    free(data);
}

void demo_extreme_values() {
    printf("\n=== Demo 5: Extreme values ===\n");
    
    int data[] = {
        2147483647,  // INT_MAX
        -2147483648, // INT_MIN
        0,
        -1,
        1,
        2147483646,
        -2147483647
    };
    int n = sizeof(data) / sizeof(data[0]);
    
    print_array("Original", data, n);
    ragesort_auto(data, n);
    print_array("Sorted", data, n);
    
    int expected[] = {-2147483648, -2147483647, -1, 0, 1, 2147483646, 2147483647};
    int correct = 1;
    for (int i = 0; i < n; i++) {
        if (data[i] != expected[i]) {
            correct = 0;
            break;
        }
    }
    printf("Extreme values test: %s\n", correct ? "PASSED" : "FAILED");
}

int main() {
    printf("RageSort Demonstration\n");
    
    demo_ragesort_auto();
    demo_ragesort_manual_inplace();
    demo_ragesort_manual_default();
    demo_large_array();
    demo_extreme_values();
    
    printf("\nAll demos completed\n");
    
    return 0;
}