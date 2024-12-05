#ifndef INCLUDE_H
#define INCLUDE_H

#define M 20

typedef struct {
    int buf[M];
    int content;
    int head;
    int tail;
} Buffer;

#endif
