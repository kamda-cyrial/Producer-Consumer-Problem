#include <iostream>
#include <thread>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define TABLE_SIZE 2
#define SHARED_MEMORY_NAME "/my_shared_memory_cpp"
#define SEM_NAME "/my_semaphore_cpp"

int *table;
sem_t *semaphore;

void produce() {
    int item = 1;
    while (true) {
        sem_wait(semaphore); // wait until there's space on the table

        // critical section
        if (table[0] == 0 || table[1] == 0) {
            int idx = (table[0] == 0) ? 0 : 1;
            table[idx] = item++;
            std::cout << "Produced item " << table[idx] << std::endl;
        }

        sem_post(semaphore); // release the semaphore
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // to avoid tight looping
    }
}

int main() {
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, TABLE_SIZE * sizeof(int));
    table = static_cast<int*>(mmap(nullptr, TABLE_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    // Initialize table
    table[0] = 0;
    table[1] = 0;

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1); // binary semaphore

    std::thread producer_thread(produce);
    producer_thread.join();

    // Cleanup
    sem_close(semaphore);
    sem_unlink(SEM_NAME);
    munmap(table, TABLE_SIZE * sizeof(int));
    close(shm_fd);
    shm_unlink(SHARED_MEMORY_NAME);

    return 0;
}
