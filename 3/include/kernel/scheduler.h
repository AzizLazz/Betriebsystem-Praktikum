#ifndef SCHEDULER_H
#define SCHEDULER_H

struct list_elem  {
    struct list_elem *prev;
    struct list_elem *next;
};

struct tcb  {
    int last_ended;
    int id;
    int active; 
    unsigned int regs[13] ; 
    unsigned int *sp ;
    unsigned int lr ;
    unsigned int pc ;
    unsigned int CPSR ;
    struct list_elem rq;                                       
};

extern struct tcb tcbs[33];
extern struct list_elem *runqueue;
extern unsigned int ids;
extern unsigned int thread_array[];

void enqueue_thread(struct tcb *thread);
struct list_elem *dequeue_thread();
void end_thread(unsigned int *ptr);
void thread_create(void (*func)(void *), const void *args, unsigned int args_size);
void switch_thread(unsigned int *ptr);
void init_idlethread();
void idle();


#endif /* SCHEDULER_H */
