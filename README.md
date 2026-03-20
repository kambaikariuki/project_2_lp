# Project 2 Assignment

Four C programs demonstrating process management, file I/O, and multi-threading concepts.


## Program 1: Pipeline Execution
Demonstrates process pipelines using fork(), pipe(), and execvp().

### Compilation & Execution
gcc pipeline.c -o pipeline
./pipeline
strace -f -o trace.log ./pipeline


## Program 2: File Copy Utilities
Compares low-level I/O (read/write) vs standard I/O (fread/fwrite).

### Compilation & Execution
gcc stdio_copy.c -o stdio_copy
gcc lowlevel_copy.c -o lowlevel_copy

### Create 100MB test file
dd if=/dev/zero of=test.bin bs=1M count=100

### Count system calls
strace -c ./stdio_copy test.bin out1.bin
strace -c ./lowlevel_copy test.bin out2.bin


## Program 3: Multi-threaded Prime Counter
Counts primes from 1 to 200,000 using 16 threads with mutex synchronization.

### Compilation & Execution

gcc prime_numbers.c -o prime_numbers -lpthread -lm
./prime_numbers


## Program 4: Multi-threaded Keyword Search
Searches for a keyword across multiple files with thread-safe output.

### Compilation & Execution
gcc search.c -o search -lpthread

### Usage: ./search_threads <keyword> <output> <files...> <threads>
./search_threads root output.txt file1.txt file2.txt file3.txt 2

