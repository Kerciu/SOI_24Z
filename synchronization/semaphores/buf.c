#include "include.h"

typedef struct Buffer {
    int buf[M];
    int content;
    int head;
    int tail;
} Buffer;
