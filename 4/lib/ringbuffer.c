#include <config.h>
#include <kernel/kprintf.h>
typedef struct {
    char* buffer;
    int head;
    int tail;
    int count;
} RingBuffer;

RingBuffer global_buffer ;
char new_buffer[UART_INPUT_BUFFER_SIZE];

void initglobal_buffer() {
    global_buffer.buffer = new_buffer;
    global_buffer.head = 0;
    global_buffer.tail = 0;
    global_buffer.count = 0;
    
}

int isBufferEmpty(RingBuffer buffer) {
    return (buffer.count == 0);
}

int isBufferFull(RingBuffer buffer) {
    return (buffer.count == UART_INPUT_BUFFER_SIZE);
}

int buffercount(RingBuffer buffer){
    return(buffer.count);
}

void enqueue(char data) {
    if (!isBufferFull(global_buffer)) {
        global_buffer.buffer[global_buffer.head] = data;
        global_buffer.head = (global_buffer.head + 1) % UART_INPUT_BUFFER_SIZE;
        global_buffer.count++;
    } else {
        kprintf("Buffer is full. Cannot enqueue.\n");
    }
}

char dequeue() {
    char data = '\0';  
    if (!isBufferEmpty(global_buffer)) {
        data = global_buffer.buffer[global_buffer.tail];
        global_buffer.tail = (global_buffer.tail + 1) % UART_INPUT_BUFFER_SIZE;
        global_buffer.count--;
    } else {
        kprintf("Buffer is empty. Cannot dequeue.\n");
    }
    return data;
}

