#ifndef ringbuffer_H
#define ringbuffer_H
#include <config.h>
typedef struct {
    char* buffer;
    int head;
    int tail;
    int count;
} RingBuffer;

void initglobal_buffer();
int isBufferEmpty();
int isBufferFull();
void enqueue(char data);
char dequeue();

#endif
