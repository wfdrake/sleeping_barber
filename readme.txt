Author: Will Drake
Date: 3/23/19

This program simulates a barber shop with x chairs and y customers (x and y being user input). The barber sleeps when there are no customers. Uses ezipc for semaphore and shared memory implementation.

To make semaphores in ezipc:
exampleSem = SEMAPHORE(type, starting value)

If type = 0, it is a counting semaphore. If type = 1, it is a binary semaphore.

Example:
exampleSem = SEMAPHORE(1, 1)
would create a binary semaphore with a starting value of 1.

