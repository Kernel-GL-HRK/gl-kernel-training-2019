# Concurrency and synchronization

## Homework

**(based on MPU6050 driver)**

_NB_:
Communication with peripheral device on i2c is quite slow asynchronous process
which responsiveness depends on the slave latency and bus utilisation.  
Thus generally it's better to be done in background process.

1. Move interaction with MPU6050 into separate thread.

2. Protect static data of the driver (`g_mpu6050_data` in the master)
for the case of concurrent access.

3. Limit interaction with MPU6050 in case of frequent requests:  
Define validity interval - if the latest read data is older than this threshold
then new data reading is performed, otherwise previously read data is returned.  
(This threshold should be configurable parameter with reasonable default.)

