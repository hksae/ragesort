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
    
    unsigned int *src = (unsigned int*)arr, *dst = (unsigned int*)buf;
    
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
        
        unsigned int* tmp = src;
        src = dst;
        dst = tmp;
    }
    
    if (src != (unsigned int*)arr) memcpy(arr, src, n * sizeof(int));
}

void ragesort(int* data, int n, int* out, int* work, int flags) {
    if (n <= 1) return;
    
    int pos_cnt[33] = {0}, neg_cnt[33] = {0};
    int maxb = 0;
    
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) {
            unsigned int abs_val = -(unsigned int)data[i];
            int len = bit_len(abs_val);
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
    
    int pos_ptr[33], neg_ptr[33];
    memcpy(pos_ptr, pos_cnt, sizeof(pos_cnt));
    memcpy(neg_ptr, neg_cnt, sizeof(neg_cnt));
    
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) {
            unsigned int abs_val = -(unsigned int)data[i];
            out[neg_ptr[bit_len(abs_val)]++] = (int)abs_val;
        } else {
            out[pos_ptr[bit_len((unsigned int)data[i])]++] = data[i];
        }
    }
    

    for (int b = maxb; b >= 0; b--) {
        int start = neg_cnt[b];
        int size = neg_ptr[b] - start;
        if (size) {
            radix_pass(out + start, size, b, work + start, cnt_buf);
            
            int l = start, r = start + size - 1;
            while (l < r) {
                unsigned int left_val = (unsigned int)out[l];
                unsigned int right_val = (unsigned int)out[r];
                out[l] = -(int)right_val;
                out[r] = -(int)left_val;
                l++; r--;
            }
            if (l == r) {
                out[l] = -(int)((unsigned int)out[l]);
            }
        }
    }
    
\


    for (int b = 0; b <= maxb; b++) {
        int start = pos_cnt[b];
        int size = pos_ptr[b] - start;
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