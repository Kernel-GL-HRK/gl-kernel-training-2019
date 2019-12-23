# Interrupts handling

## Homework

**(based on MPU6050 driver)**

1. Setup handling of MPU6050 interrupts.
- Configure interrupt request on MPU6050 (e.g. motion detection).
- Connect interrupt output of the MPU6050 with Orange-Pi GPIO.
- Configure GPIO as input.
- Register handler for the MPU6050 interrupt.

2. Implement deferred interrupt handler (e.g. in custom workqueue) to handle the request.
- Read new measurements.
- Implement measurements processing (e.g. the following or per your preference).
  - filtering (e.g. moving average);
  - integrating (for acc to get speed);
  - determine trends;
  - etc.

3. Provide additional interface (e.g. in procfs) to read the processed data at once.

_Note: Remember to synchronize all concurrent operations_
