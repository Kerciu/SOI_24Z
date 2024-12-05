#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "buf.h"

Buffer buffer = { .content = 0, .head = 0, .tail = 0};

sem_t empty;
sem_t full;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
    return 0;
}