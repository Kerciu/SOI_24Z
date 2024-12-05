#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buf.h"

#define ITERS M


Buffer buffer = { .content = 0, .head = 0, .tail = 0};

sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



void producent(void* arg)
{
    for (int i = 0; i < ITERS; ++i)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        put(i, &buffer);

        pthread_spin_unlock(&mutex);
        sem_post(&full);
    }
}

void consument(void* arg)
{
    for (int i = 0; i < ITERS; ++i)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        int thing = get(&buffer);

        pthread_spin_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(int argc, char** argv)
{
    return 0;
}