Producer-Consumer Problem using Shared Memory and Semaphores
-----------------------------------------------------------

Description:
The producer generates items and puts them on a shared table. The consumer picks up items from this table. The table can only hold two items at a time. When the table is full, the producer waits, and when it's empty, the consumer waits. Semaphores are used to synchronize the producer and consumer.

# Compilation:
```
g++ producer.cpp -pthread -lrt -o producer
g++ consumer.cpp -pthread -lrt -o consumer
 ```


# Execution:
In 2 different terminals, execute these commands
```
./producer
./consumer
```
