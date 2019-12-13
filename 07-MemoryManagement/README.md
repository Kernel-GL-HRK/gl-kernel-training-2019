# Memory management

## Homework
1. Create user-space program which allocates buffers with sizes
  2^x and x^2 + 1 for x in range 0 to 64 using functions:
  **malloc, calloc, alloca**.
  Measure time of each allocation/freeing.
  2^x means x power of 2 in this task.

2. Create kernel module and test allocation/freeing time for functions:
  **kmalloc, kzmalloc, vmalloc, get_free_pages**.
  Measure the time of each allocation/freeing.
  The results should be presented in text file table with followed columns:
  Buffer size, allocation time(min, average, max),
  freeing time (min, average, max).
  Size unit is 1 byte, time unit is 1 ns.

