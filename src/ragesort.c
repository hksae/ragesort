/**
 * @file ragesort.c
 * @brief RageSort implementation - high-performance radix sort for 32-bit integers.
 */

#include "../include/ragesort.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

void insertion_sort(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i], j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void radix_pass(int* arr, int n, int bits, int* buf, int* cnt) {
    if (n < 64) {
        insertion_sort(arr, n);
        return;
    }
    
    unsigned int flip = (bits == 32) ? (1U << 31) : 0;
    if (flip) for (int i = 0; i < n; i++) arr[i] ^= flip;
    
    int *src = arr, *dst = buf;
    
    for (int shift = 0; shift < bits; shift += 8) {
        if (shift >= 32) break;
        memset(cnt, 0, 256 * sizeof(int));
        for (int i = 0; i < n; i++) cnt[(src[i] >> shift) & 0xFF]++;
        
        int sum = 0;
        for (int i = 0; i < 256; i++) {
            int t = cnt[i];
            cnt[i] = sum;
            sum += t;
        }
        
        for (int i = 0; i < n; i++) {
            int b = (src[i] >> shift) & 0xFF;
            dst[cnt[b]++] = src[i];
        }
        
        int* tmp = src;
        src = dst;
        dst = tmp;
    }
    
    if (src != arr) memcpy(arr, src, n * sizeof(int));
    if (flip) for (int i = 0; i < n; i++) arr[i] ^= flip;
}

void ragesort(int* data, int n, int* out, int* work, int flags) {
    if (n <= 1) return;
    
    int pos_cnt[35] = {0}, neg_cnt[35] = {0};
    int maxb = 0;
    
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) {
            int len = bit_len(-(unsigned int)data[i]);
            neg_cnt[len]++;
            if (len > maxb) maxb = len;
        } else {
            int len = bit_len((unsigned int)data[i]);
            pos_cnt[len]++;
            if (len > maxb) maxb = len;
        }
    }
    
    int cnt_buf[256];
    
    int off = 0;
    for (int b = maxb; b >= 0; b--) {
        if (neg_cnt[b]) {
            int t = neg_cnt[b];
            neg_cnt[b] = off;
            off += t;
        } else {
            neg_cnt[b] = off;
        }
    }
    int neg_base = off;
    
    for (int b = 0; b <= maxb; b++) {
        if (pos_cnt[b]) {
            int t = pos_cnt[b];
            pos_cnt[b] = off;
            off += t;
        } else {
            pos_cnt[b] = off;
        }
    }
    
    pos_cnt[maxb + 1] = n;
    for (int b = maxb + 2; b < 35; b++) pos_cnt[b] = n;
    neg_cnt[-1 + 1] = neg_base;
    for (int b = -2 + 1; b >= 0; b--) neg_cnt[b] = neg_base;
    
    int pos_ptr[35], neg_ptr[35];
    memcpy(pos_ptr, pos_cnt, sizeof(pos_cnt));
    memcpy(neg_ptr, neg_cnt, sizeof(neg_cnt));
    
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) {
            out[neg_ptr[bit_len(-(unsigned int)data[i])]++] = -(unsigned int)data[i];
        } else {
            out[pos_ptr[bit_len((unsigned int)data[i])]++] = data[i];
        }
    }
    
    for (int b = maxb; b >= 0; b--) {
        int start = neg_cnt[b];
        int size = neg_cnt[b - 1 + 1] - start;
        if (size) {
            radix_pass(out + start, size, b, work + start, cnt_buf);
            
            int l = start, r = start + size - 1;
            while (l < r) {
                int tmp = -out[l];
                out[l] = -out[r];
                out[r] = tmp;
                l++; r--;
            }
            if (l == r) out[l] = -out[l];
        }
    }
    
    for (int b = 0; b <= maxb; b++) {
        int start = neg_base + pos_cnt[b];
        int size = pos_cnt[b + 1] - pos_cnt[b];
        if (size) radix_pass(out + start, size, b, work + start, cnt_buf);
    }
    
    if (!(flags & RAGESORT_INPLACE) && out != data) {
        memcpy(data, out, n * sizeof(int));
    }
}

void ragesort_auto(int* data, int n) {
    if (n <= 1) return;
    
    int* out = (int*)malloc(n * sizeof(int));
    int* work = (int*)malloc(n * sizeof(int));
    
    if (!out || !work) {
        free(out);
        free(work);
        return;
    }
    
    ragesort(data, n, out, work, RAGESORT_DEFAULT);
    
    free(out);
    free(work);
}