/**
 * @file test_correctness.c
 * @brief Correctness tests for RageSort algorithm.
 */

#include "../include/ragesort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAILED: %s (line %d)\n", msg, __LINE__); \
            return 1; \
        } \
    } while(0)

#define TEST_SIZE 10000

int compare_int(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

void print_array(int* arr, int n) {
    for (int i = 0; i < n && i < 20; i++) {
        printf("%d ", arr[i]);
    }
    if (n > 20) printf("...");
    printf("\n");
}

int test_empty() {
    int data[] = {42};
    ragesort_auto(data, 0);
    return 0;
}

int test_single() {
    int data[] = {42};
    ragesort_auto(data, 1);
    ASSERT(data[0] == 42, "Single element should remain unchanged");
    return 0;
}

int test_sorted_ascending() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = i;
        expected[i] = i;
    }
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Already sorted ascending");
    return 0;
}

int test_sorted_descending() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = TEST_SIZE - i;
        expected[i] = i + 1;
    }
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Descending array");
    return 0;
}

int test_all_equal() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = 42;
        expected[i] = 42;
    }
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "All equal elements");
    return 0;
}

int test_all_zero() {
    int data[TEST_SIZE] = {0};
    int expected[TEST_SIZE] = {0};
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "All zeros");
    return 0;
}

int test_negative_only() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = -i;
        expected[i] = -TEST_SIZE + i + 1;
    }
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Negative only");
    return 0;
}

int test_positive_only() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = i;
        expected[i] = i;
    }
    
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Positive only");
    return 0;
}

int test_mixed() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = (i % 2 == 0) ? i : -i;
        expected[i] = data[i];
    }
    
    qsort(expected, TEST_SIZE, sizeof(int), compare_int);
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Mixed positive/negative");
    return 0;
}

int test_random() {
    int data[TEST_SIZE];
    int expected[TEST_SIZE];
    
    srand(1337);
    for (int i = 0; i < TEST_SIZE; i++) {
        data[i] = (rand() % 2000000000) - 1000000000;
        expected[i] = data[i];
    }
    
    qsort(expected, TEST_SIZE, sizeof(int), compare_int);
    ragesort_auto(data, TEST_SIZE);
    ASSERT(memcmp(data, expected, sizeof(data)) == 0, "Random array");
    return 0;
}

int test_extreme_values() {
    int data[] = {INT_MAX, INT_MIN, 0, -1, 1, INT_MAX - 1, INT_MIN + 1};
    int expected[] = {INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX - 1, INT_MAX};
    int n = sizeof(data) / sizeof(data[0]);
    
    ragesort_auto(data, n);
    
    for (int i = 0; i < n; i++) {
        if (data[i] != expected[i]) {
            printf("  Expected: ");
            for (int j = 0; j < n; j++) printf("%d ", expected[j]);
            printf("\n");
            printf("  Got:      ");
            for (int j = 0; j < n; j++) printf("%d ", data[j]);
            printf("\n");
            ASSERT(0, "Extreme values (INT_MIN/INT_MAX) - element mismatch");
        }
    }
    return 0;
}

int test_inplace_flag() {
    int n = TEST_SIZE;
    int* data = (int*)malloc(n * sizeof(int));
    int* out = (int*)malloc(n * sizeof(int));
    int* work = (int*)malloc(n * sizeof(int));
    int* expected = (int*)malloc(n * sizeof(int));
    int* original_data = (int*)malloc(n * sizeof(int));
    
    if (!data || !out || !work || !expected || !original_data) {
        free(data); free(out); free(work); free(expected); free(original_data);
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        data[i] = n - i;
        original_data[i] = data[i];
    }
    
    for (int i = 0; i < n; i++) {
        expected[i] = i + 1;
    }
    
    memset(out, 0xAA, n * sizeof(int));
    
    ragesort(data, n, out, work, RAGESORT_INPLACE);
    
    for (int i = 0; i < n - 1; i++) {
        ASSERT(out[i] <= out[i + 1], "INPLACE: out buffer should be sorted");
    }
    ASSERT(memcmp(out, expected, n * sizeof(int)) == 0, "INPLACE: out buffer contains correct sorted order");
    
    ASSERT(memcmp(data, original_data, n * sizeof(int)) == 0, "INPLACE: original data array must remain unchanged");
    
    free(data);
    free(out);
    free(work);
    free(expected);
    free(original_data);
    
    return 0;
}

int main() {
    printf("Running RageSort correctness tests...\n\n");
    
    int (*tests[])(void) = {
        test_empty,
        test_single,
        test_sorted_ascending,
        test_sorted_descending,
        test_all_equal,
        test_all_zero,
        test_negative_only,
        test_positive_only,
        test_mixed,
        test_random,
        test_extreme_values,
        test_inplace_flag
    };
    
    const char* names[] = {
        "Empty array",
        "Single element",
        "Already sorted ascending",
        "Already sorted descending",
        "All equal elements",
        "All zeros",
        "Negative only",
        "Positive only",
        "Mixed positive/negative",
        "Random array",
        "Extreme values",
        "In-place flag"
    };
    
    int passed = 0;
    int failed = 0;
    
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        printf("Test %d: %s... ", i + 1, names[i]);
        fflush(stdout);
        
        if (tests[i]() == 0) {
            printf("PASSED\n");
            passed++;
        } else {
            printf("FAILED\n");
            failed++;
        }
    }
    printf("Results: %d passed, %d failed\n", passed, failed);
    
    return failed > 0 ? 1 : 0;
}