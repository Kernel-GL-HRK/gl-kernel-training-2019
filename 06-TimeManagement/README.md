# Internal Kernel API (Time Management)

## Homework

1. User space. Implement program which shows absolute time in user space.
Pull request should contain the commit with source code and
generated output in text format.

2. Kernel space. 
  a) Implement kernel module with API in sysfs,
     which show relation time in maximum possible resolution 
     passed since previous read of it.
  b) Implement kernel module with API in sysfs which shows absolute time of
     previous reading with maximum resolution like ‘400.123567’ seconds.
  c) Implement kernel module with API in sysfs which shows average processor
     load updated once in a second.
  Pull request should contain the commit with source code and
  text output from sysfs.
