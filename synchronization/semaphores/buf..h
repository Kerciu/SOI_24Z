#ifndef INCLUDE_H
#define INCLUDE_H

#define M 20

typedef struct {
    int buf[M];
    int max_size = M;
    int content;
    int head;
    int tail;
} Buffer;

void* put(int thing, Buffer* buffer);

int get(Buffer* buffer);

#endif
