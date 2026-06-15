# RageSort


A blazing fast, hardware-optimized hybrid Radix Sort written in pure C (C99) for 32-bit signed integers.

**RageSort** is a non-comparison-based sorting pipeline engineered to extract maximum performance from modern CPU architectures. Instead of processing entire arrays with a fixed number of radix passes, RageSort profiles the dataset in a single linear scan and shards elements into adaptive buckets based on their actual bit-length using hardware-accelerated MSB detection (`__builtin_clz`).

## Features

- **Blazing Fast** — Up to **3x faster** than standard library `qsort` on short integers and smaller data distributions.
- **Branchless Bit Extraction** — Utilizes the compiler intrinsic `__builtin_clz` combined with a zero-handling bitwise hack (`x | 1`) to eliminate CPU branch mispredictions.
- **Adaptive Fallback** — Smoothly transitions to an optimized Insertion Sort for tiny micro-clusters ($N < 64$) to eliminate `memset` and buffer-swapping overhead.
- **Flexible API** — Offers a manual buffer-management pipeline for tight hot paths (e.g., game engines, database kernels) and an automatic wrapper for everyday convenience.
- **Zero Allocations on Hot Paths** — The core function operates entirely out-of-place using pre-allocated, caller-provided memory workspaces.
- **Memory Efficient In-Place Mode** — Skips the final array-copying step (`memcpy`) when processing chunks, keeping sorted structures directly inside the output target.

---

## Quick Start

Ensure you include the main tracking header file:

```c
#include "ragesort.h"

int main() {
    int data[] = {5, -3, 42, -17, 0, 100, -42, 7, -100, 23};
    int n = sizeof(data) / sizeof(data[0]);

    // High-level automatic execution wrapper
    ragesort_auto(data, n);
    
    return 0;
}
```

---

## API Reference

### ragesort_auto
```c
void ragesort_auto(int* data, int n);
```
Sorts a signed integer array inside its original bounds. This wrapper automatically handles intermediate buffer allocation on the heap and safely cleans up memory afterwards.
* **Safety Feature:** Built-in Out-Of-Memory (OOM) tracking; if dynamic allocation fails, it fails gracefully without leaking memory or crashing the host program.

### ragesort
```c
void ragesort(int* data, int n, int* out, int* work, int flags);
```
Low-level, raw pipeline designed for maximum performance constraints. Requires external configuration tracking arrays.
* **Parameters:**
  * `data` — Pointer to the source integer array.
  * `n` — Number of elements to be processed.
  * `out` — Scatter buffer destination (must be sized $\ge N$).
  * `work` — Scratchpad radix pass auxiliary workspace (must be sized $\ge N$).
  * `flags` — Configuration mask bit value (e.g., `RAGESORT_DEFAULT` or `RAGESORT_INPLACE`).
* **Flags:**
  * `RAGESORT_DEFAULT` — The finalized result is automatically committed and copied back into the `data` array.
  * `RAGESORT_INPLACE` — Bypasses the final `memcpy`. The sorted data is left in the `out` buffer, and the source `data` array remains unmodified.

---

## 🛠 Building & Automation

### 1. Modern CMake Pipeline (Cross-Platform)
To build the library, correctness tests, performance profiling tools, and functional demo systems, run:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### 2. Makefile Wrapper (Linux / macOS Execution)
For developer convenience, a fast Makefile wrapper is supplied to trigger building tasks via shortened commands:

```bash
make          # Default target: builds library and all executables
make test     # Compiles and runs complete correctness tests (with Valgrind if installed)
make benchmark # Compiles and fires up performance profiling benchmarks
make demo     # Compiles and executes live behavioral demonstrations
make format   # Automatically standardizes layout rules using clang-format
make clean    # Safely purges compiled target binaries and intermediate build folders
make install  # Installs library structures and tracked headers into system directories (/usr/local)
```

---

## 📊 Performance Benchmarks

*Measurements gathered across 10,000,000 array instances under automated compiler optimizations (`-O3 -march=native -flto`). Execution timing scaled lower is faster.*

### Random 32-bit Integer Distribution
*Spans full numerical range across wide random clusters ($-10^9 \dots 10^9$).*

| Algorithm | Processing Time (sec) | Scaling Factor |
| :--- | :--- | :--- |
| Standard `qsort` | 1.85s | 1.00x (Baseline) |
| **RageSort** | **1.12s** | **1.65x Faster** |

### Small Numbers Distribution
*Optimized narrow data layouts simulating small index bounds ($[-128 \dots 127]$).*

| Algorithm | Processing Time (sec) | Scaling Factor |
| :--- | :--- | :--- |
| Standard `qsort` | 1.42s | 1.00x (Baseline) |
| **RageSort** | **0.48s** | **2.96x Faster** |

---

## 📜 License

This project is licensed under the terms of the **MIT License**. See the [LICENSE](LICENSE) file for details.
