/**
 * @file benchmark.c
 * @brief Performance benchmark for RageSort vs qsort.
 */

#include "../include/ragesort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    static double get_time() {
        LARGE_INTEGER freq, count;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&count);
        return (double)count.QuadPart / (double)freq.QuadPart;
    }
#else
    #include <sys/time.h>
    static double get_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }
#endif

#define BENCHMARK_SIZE 10000000
#define RANDOM_RANGE 2000000000

int compare_int(const void* a, const void* b) {
    int va = *(const int*)a;
    int vb = *(const int*)b;
    return (va > vb) - (va < vb);
}

double benchmark_qsort(int* data, int n) {
    double start = get_time();
    qsort(data, n, sizeof(int), compare_int);
    double end = get_time();
    return end - start;
}

double benchmark_ragesort_auto(int* data, int n) {
    double start = get_time();
    ragesort_auto(data, n);
    double end = get_time();
    return end - start;
}

double benchmark_ragesort_manual(int* data, int n, int* out, int* work, int flags) {
    double start = get_time();
    ragesort(data, n, out, work, flags);
    double end = get_time();
    return end - start;
}

int run_benchmark(const char* name, int* data, int n, int iterations) {
    printf("\n=== %s (n=%d, iter=%d) ===\n", name, n, iterations);
    
    int* copy1 = (int*)malloc(n * sizeof(int));
    int* copy2 = (int*)malloc(n * sizeof(int));
    int* out = (int*)malloc(n * sizeof(int));
    int* work = (int*)malloc(n * sizeof(int));
    
    if (!copy1 || !copy2 || !out || !work) {
        printf("Failed to allocate memory for %s\n", name);
        free(copy1); free(copy2); free(out); free(work);
        return 1;
    }
    
    double qsort_time = 0.0;
    double ragesort_auto_time = 0.0;
    double ragesort_manual_time = 0.0;
    
    for (int iter = 0; iter < iterations; iter++) {
        memcpy(copy1, data, n * sizeof(int));
        qsort_time += benchmark_qsort(copy1, n);
        
        memcpy(copy2, data, n * sizeof(int));
        ragesort_auto_time += benchmark_ragesort_auto(copy2, n);
        
        memcpy(copy1, data, n * sizeof(int));
        ragesort_manual_time += benchmark_ragesort_manual(copy1, n, out, work, RAGESORT_DEFAULT);
    }
    
    qsort_time /= iterations;
    ragesort_auto_time /= iterations;
    ragesort_manual_time /= iterations;
    
    printf("qsort:           %.4f sec\n", qsort_time);
    printf("ragesort_auto:   %.4f sec (%.2fx %s)\n", 
           ragesort_auto_time, 
           qsort_time / ragesort_auto_time,
           ragesort_auto_time < qsort_time ? "faster" : "slower");
    printf("ragesort_manual: %.4f sec (%.2fx %s)\n",
           ragesort_manual_time,
           qsort_time / ragesort_manual_time,
           ragesort_manual_time < qsort_time ? "faster" : "slower");
    
    free(copy1);
    free(copy2);
    free(out);
    free(work);
    
    return 0;
}

int main() {
    printf("RageSort Benchmark\n");
    
    const int n = BENCHMARK_SIZE;
    const int iterations = 3;
    
    int* random_data = (int*)malloc(n * sizeof(int));
    int* small_data = (int*)malloc(n * sizeof(int));
    int* ascending_data = (int*)malloc(n * sizeof(int));
    int* descending_data = (int*)malloc(n * sizeof(int));
    
    if (!random_data || !small_data || !ascending_data || !descending_data) {
        printf("Failed to allocate benchmark arrays\n");
        free(random_data); free(small_data); free(ascending_data); free(descending_data);
        return 1;
    }
    
    srand(1337);
    for (int i = 0; i < n; i++) {
        random_data[i] = (rand() % RANDOM_RANGE) - (RANDOM_RANGE / 2);
        small_data[i] = (rand() % 256) - 128;
        ascending_data[i] = i;
        descending_data[i] = n - i;
    }
    
    printf("\nSystem info:\n");
    printf("Array size: %d elements (%d MB)\n", n, (n * sizeof(int)) / (1024 * 1024));
    printf("Iterations per test: %d\n", iterations);
    
    if (run_benchmark("Random 32-bit integers", random_data, n, iterations)) return 1;
    if (run_benchmark("Small numbers (-128..127)", small_data, n, iterations)) return 1;
    if (run_benchmark("Already ascending", ascending_data, n, iterations)) return 1;
    if (run_benchmark("Already descending", descending_data, n, iterations)) return 1;
    
    free(random_data);
    free(small_data);
    free(ascending_data);
    free(descending_data);
    
    printf("Benchmark complete\n");
    
    return 0;
}