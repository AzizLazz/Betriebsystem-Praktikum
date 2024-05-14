#ifndef ringbuffer_H
#define ringbuffer_H
#include <config.h>
typedef struct {
    char* buffer;
    int head;
    int tail;
    int count;
} RingBuffer;

extern RingBuffer global_buffer ;
extern char new_buffer[UART_INPUT_BUFFER_SIZE];
int buffercount(RingBuffer buffer);

void initglobal_buffer();
int isBufferEmpty(RingBuffer buffer);
int isBufferFull(RingBuffer buffer);
void enqueue(char data);
char dequeue();

#endif
