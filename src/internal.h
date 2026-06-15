/**
 * @file internal.h
 * @brief Internal utilities for RageSort algorithm.
 * 
 * This header contains private functions and macros used internally by the 
 * RageSort implementation. Not intended for public use.
 */

#ifndef RAGESORT_INTERNAL_H
#define RAGESORT_INTERNAL_H

#include <string.h>

/**
 * @brief Branchless bit-length calculation for 32-bit unsigned integers.
 * 
 * Returns the position of the highest set bit (1-indexed).
 * For x = 0, returns 1 (treated as bit-length 1 for sorting purposes).
 * 
 * @param x Input value
 * @return Number of bits required to represent x (1..32)
 */
static inline int bit_len(unsigned int x) {
    return 32 - __builtin_clz(x | 1);
}

/**
 * @brief Insertion sort for small arrays.
 * 
 * Used as a fallback when array size falls below threshold (64).
 * In-place, stable, O(n²) but fast for tiny datasets.
 * 
 * @param arr Array to sort
 * @param n Number of elements
 */
void insertion_sort(int* arr, int n);

/**
 * @brief LSD radix pass for a single byte.
 * 
 * Sorts an array by a specific byte (0-3) using counting sort.
 * Handles sign bit correction for the most significant byte.
 * 
 * @param arr   Array to sort (will be modified)
 * @param n     Number of elements
 * @param bits  Bit-length of values (for flip mask calculation)
 * @param buf   Temporary buffer (size >= n)
 * @param cnt   Counting array (size 256, caller-provided for stack allocation)
 */
void radix_pass(int* arr, int n, int bits, int* buf, int* cnt);

#endif /* RAGESORT_INTERNAL_H */