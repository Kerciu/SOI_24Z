#include "buf.h"

void* put(int thing, Buffer* buffer)
{
    buffer->data[buffer->tail] = thing;
    buffer->tail = (buffer->tail + 1) % buffer->max_size;
    ++buffer->count;
}

int get(Buffer* buffer)
{
    int thing = buffer->data[buffer->head];
    buffer->head = (buffer->head + 1) % buffer->max_size;
    --buffer->count;
    return thing;
}