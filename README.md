# Parallel MD5 Hashing Tool with MPI

This is a C-based parallel MD5 hashing tool that uses **MPI (Message Passing Interface)** to distribute computation across multiple processes. It is designed for performance and portability using standard C libraries and a custom MD5 implementation.

##  Note

This project demonstrates a conceptual approach to parallelism in cryptographic hash functions using MD5. While MD5 is relatively simpler and not secure for modern cryptography, this parallel implementation sets the foundation for more complex algorithms like SHA-1, SHA-256, or SHA-512, which involve more intricate bitwise operations and larger buffer states.

The idea is to parallelize the input buffer processing, distributing chunks of data to multiple processes—each handling parts of the hashing pipeline or different inputs. This mimics how modern cryptographic systems handle data in parallel for performance optimization.

##  Features

- Parallel computation using MPI
- Custom implementation of MD5 hashing
- Utilizes standard C libraries including `math.h`, `string.h`, etc.
- Written in pure C

##  Requirements

- GCC (GNU Compiler Collection)
- MPI (e.g., OpenMPI or MPICH)
- Math library (`libm`)
- Custom MD5 header and implementation files (`md5.h` and `md5.c`)

###  Install Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential libopenmpi-dev openmpi-bin

```

##  Build Instructions

Compile the project using `mpicc`, the MPI C compiler wrapper:

```
mpicc -o md5_tool main.c md5.c -lm
```
- `main.c`: main program using MPI and MD5.
- `md5.c`: The implementation of MD5 functions.
    
##  Run Instructions

Run the compiled executable using `mpirun` with the number of processes you want:

```
mpirun -np 4 ./md5_tool
```

Replace `4` with the desired number of parallel processes.

##  Notes

- Ensure that `md5.h` and `md5.c` are correctly implemented and accessible in your working directory.
- The `md5.h` file must declare all necessary functions used in `main.c`, and `md5.c` must define them.
    

##  License

This project is licensed under the MIT License.

##  Contributing

Feel free to contribute by submitting issues or pull requests to improve functionality or performance.

--- Let me know if you'd like to include a sample input/output or description of how the parallelism works!

