# Parallel MD5 Hashing Tool with MPI

This is a C-based parallel MD5 hashing tool that uses **MPI (Message Passing Interface)** to distribute computation across multiple processes. It is designed for performance and portability using standard C libraries and a custom MD5 implementation.

## 🧾 Features

- Parallel computation using MPI
- Custom implementation of MD5 hashing
- Utilizes standard C libraries including `math.h`, `string.h`, etc.
- Written in pure C

## ⚙️ Requirements

- GCC (GNU Compiler Collection)
- MPI (e.g., OpenMPI or MPICH)
- Math library (`libm`)
- Custom MD5 header and implementation files (`md5.h` and `md5.c`)

### ✅ Install Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential libopenmpi-dev openmpi-bin

### 🛠️ **Build Instructions**
Compile the project using mpicc, the MPI C compiler wrapper:
