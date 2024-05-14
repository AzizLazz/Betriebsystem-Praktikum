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
extern struct list_elem *uartwaitqueue;
extern struct list_elem *svcwaitqueue;
extern unsigned int ids;
extern unsigned int thread_array[];

void printrunqueue();

void enqueue_thread(struct tcb *thread, int queuetype);
struct list_elem *dequeue_thread(int queuetype);
void end_thread(unsigned int *ptr);
void thread_create(void (*func)(void *), const void *args, unsigned int args_size);
void switch_thread(unsigned int *ptr, int queuetype);
void sleepdequeue(unsigned int *ptr);
void maindequeue(unsigned int *ptr);
void init_idlethread();
void init_mainthread();
void idle();
void uartenqueue();
void sleepenqueue(unsigned int *ptr);
int uartiswaiting();
void enqueue_thread_at_front(struct tcb *thread);
void enqueue_thread_at_second(struct tcb *thread);
void setcomparevalue_tcb(unsigned int x);
#endif /* SCHEDULER_H */
