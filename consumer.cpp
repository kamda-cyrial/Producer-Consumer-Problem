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

void consume() {
    while (true) {
        sem_wait(semaphore); // wait until there's an item on the table

        // critical section
        for (int i = 0; i < TABLE_SIZE; ++i) {
            if (table[i] != 0) {
                std::cout << "Consumed item " << table[i] << std::endl;
                table[i] = 0;
                break;
            }
        }

        sem_post(semaphore); // release the semaphore
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // to avoid tight looping
    }
}

int main() {
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    table = static_cast<int*>(mmap(nullptr, TABLE_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    semaphore = sem_open(SEM_NAME, 0); // Get the existing semaphore

    std::thread consumer_thread(consume);
    consumer_thread.join();

    // Cleanup
    sem_close(semaphore);
    munmap(table, TABLE_SIZE * sizeof(int));
    close(shm_fd);

    return 0;
}
