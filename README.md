# CIC-Interpolation

High-performance 2D interpolation using the Cloud-in-Cell (CIC) scheme, parallelized with OpenMP for shared-memory environments. Designed for applications in scientific visualization, finite element methods, and medical imaging.

---

## 🚀 Features

* ✅ **Bilinear interpolation** using the **Cloud-in-a-Cell (CIC)** method
* ⚡ High-performance parallel execution using **OpenMP**
* 🧮 Supports large-scale structured mesh grids
* 📊 Useful for domains like scientific computing, finite element analysis, and medical data processing
* 🧩 Multiple optimization strategies included:

  * Serial (baseline)
  * Cache-aware tiling/blocking
  * Spatial sorting via Morton (Z-order)
  * Thread-private buffer parallelization

---

## 📦 Prerequisites

* C compiler with OpenMP support (e.g., GCC, Clang)
* `make` build tool
* Unix-like environment (Linux, macOS)

---

## 🛠️ Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/cic-interpolation.git
cd cic-interpolation

# Compile all variants
make all
```

This will produce the following executables:

* `serial_cic`      — baseline serial implementation
* `cic_tiled`       — cache-blocked (tiling) version
* `cic_morton`      — Morton-order spatial sorting version

---

## ▶️ Usage

All executables take the same arguments:

```bash
./<executable> <input_binary> <num_threads>
```

* `<input_binary>`: Path to the binary file containing:

  1. `int NX, NY`          — grid dimensions
  2. `int NUM_PTS`        — number of interpolation points
  3. `double x, y` pairs  — point coordinates
* `<num_threads>`: Number of OpenMP threads (for serial\_cic, ignored and runs single-threaded)

### Examples

```bash
# Serial baseline
time ./serial_cic input.bin 1

# Tiled version (8 threads)
time OMP_NUM_THREADS=8 ./cic_tiled   input.bin 8

# Morton-sorted version (8 threads)
time OMP_NUM_THREADS=8 ./cic_morton  input.bin 8
```

---

## 📊 Performance Comparison

| Method               | Configuration | Time (s) |
| :------------------- | :------------ | :------- |
| Serial CIC           | 1 thread      | 1.234    |
| Tiled / Blocking CIC | 8 threads     | 0.276    |
| Morton-sorted CIC    | 8 threads     | 0.295    |

> *Results will vary depending on hardware and problem size.*

---

## 📁 Project Structure

```
├── src/                 # Source code
│   ├── serial_cic.c
│   ├── cic_tiled.c
│   ├── cic_morton.c
│   └── Makefile         # Build rules
├── input.bin            # Sample input (gitignored; generate separately)
├── benchmarks/          # Scripts for automated benchmarking
└── README.md            # This file
```

---

## 🔮 Future Work

* Support 3D CIC interpolation
* GPU acceleration via CUDA/OpenCL
* Adaptive mesh refinement (AMR)
* Integration with higher-level frameworks (e.g., PETSc)

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 📝 Acknowledgments

Inspired by classical Particle-in-Cell and grid interpolation methods used in astrophysics and fluid dynamics research.
