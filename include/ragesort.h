/**
 * @file ragesort.h
 * @brief Blazing fast, hardware-optimized hybrid Radix Sort for 32-bit signed integers.
 * @author hksae
 * 
 * RageSort is a high-performance, non-comparison-based sorting pipeline optimized 
 * for modern CPU architectures. It leverages hardware-accelerated bit-length profiling 
 * (branchless MSB extraction) and dynamically adapts radix pass depth to the dataset.
 */

#ifndef RAGESORT_H
#define RAGESORT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Configuration Flags
 * @{
 */
/** @brief Default execution mode. The sorted result is always copied back into the source 'data' array. */
#define RAGESORT_DEFAULT  0

/** 
 * @brief In-place sorting mode.
 * Sorts the 'data' array directly within the 'out' buffer, bypassing the final memcpy.
 * When using this flag, 'out' must point to the same memory region as 'data'.
 */
#define RAGESORT_INPLACE  (1 << 0)
/** @} */

/**
 * @brief Low-level core sorting pipeline requiring externally pre-allocated buffers.
 * 
 * Recommended for hot paths and performance-critical systems where auxiliary memory 
 * can be allocated once at a higher architecture level (e.g., game engines, database kernels).
 * 
 * @param[in,out] data  Pointer to the source array of 32-bit signed integers.
 * @param[in]     n     Number of elements in the array.
 * @param[out]    out   User-allocated destination scatter buffer (must be size >= n).
 * @param[out]    work  Auxiliary scratchpad workspace buffer for radix passes (must be size >= n).
 * @param[in]     flags Configuration bitmask (e.g., ::RAGESORT_DEFAULT or ::RAGESORT_INPLACE).
 * 
 * @note If n <= 1, the function returns immediately without modifying any buffers.
 * @warning The auxiliary buffers 'out' and 'work' MUST be allocated with a size of at least 'n'. 
 *          Failure to do so will result in memory corruption or segmentation faults.
 */
void ragesort(int* data, int n, int* out, int* work, int flags);

/**
 * @brief Automated safe wrapper over the core sorting pipeline.
 * 
 * Dynamically allocates transient heap buffers required by the algorithm and 
 * guarantees their proper deallocation upon completion. The sorted result is 
 * always returned directly inside the original 'data' array.
 * 
 * @param[in,out] data Pointer to the array to be sorted.
 * @param[in]     n    Number of elements in the array.
 * 
 * @note Contains built-in Out-Of-Memory (OOM) protection; fails gracefully if allocation fails.
 */
void ragesort_auto(int* data, int n);

#ifdef __cplusplus
}
#endif

#endif /* RAGESORT_H */