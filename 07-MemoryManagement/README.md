# Homework:

1. Create user-space program which allocates buffers x = 0..64 with 
sizes 2^x and 2^x + 1
- using functions: malloc, calloc, alloca
- Measure time of each allocation/freeing

2. Create kernel module and test allocation/freeing time for functions:
kmalloc, kzmalloc, vmalloc, get_free_pages
- check time of each allocation and freeing

3. Results in should be in text file table with columns:
Buffer size, Allocation time, min, average, max, ns, freeing time min,
av, max.
