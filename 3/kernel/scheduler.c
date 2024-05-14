#include <main.h>
#include <kernel/asm_hilfsfunktionen.h>
#include <stddef.h>
#include <kernel/handler_asm.h>

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *mptr = (ptr);    \
        (type *)( (char *)mptr - offsetof(type,member) );})


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

struct tcb tcbs[33];                                            
struct list_elem *runqueue = (void*)0;                                     

unsigned int ids = 0;

unsigned int thread_array[] = {
    0x08FF2000, 0x08FF3000, 0x08FF4000, 0x08FF5000, 0x08FF6000,
    0x08FF7000, 0x08FF8000, 0x08FF9000, 0x08FFA000, 0x08FFB000,
    0x08FFC000, 0x08FFD000, 0x08FFE000, 0x08FFF000, 0x09000000,
    0x09001000, 0x09002000, 0x09003000, 0x09004000, 0x09005000,
    0x09006000, 0x09007000, 0x09008000, 0x09009000, 0x0900A000,
    0x0900B000, 0x0900C000, 0x0900D000, 0x0900E000, 0x0900F000,
    0x09010000, 0x09011000
};

void enqueue_thread(struct tcb *thread) {
    if (runqueue == (void*)0) {
        thread->rq.prev = (void*)0;
        thread->rq.next = (void*)0;
        runqueue = &thread->rq;
    } else {
        struct list_elem *lastElem = runqueue;
        while (lastElem->next != (void*)0) {
            lastElem = lastElem->next;
        }

        lastElem->next =  &thread->rq;
        thread->rq.prev = lastElem;
        thread->rq.next = (void*)0;
    }
}

void idle(){
    while (1)
    {
        asm("");
    }
    
}

struct list_elem *dequeue_thread() {
    if (runqueue == (void*)0) {
        return (void*)0;
    }

    struct list_elem *firstElem = runqueue;
    runqueue = runqueue->next;
    if (runqueue != (void*)0) {
        runqueue->prev = (void*)0;
        
    }
    return firstElem;
}

void end_thread(unsigned int *ptr){
    struct list_elem *temp = runqueue;
    struct tcb *element = container_of(temp, struct tcb, rq);

            for(int j = 0;j < 13;j++){
                element->regs[j] = 0 ; 
            }

            element->id= 0 ;
            element->active = 0;
            element->sp = 0 ;
            element->lr= 0 ;
            element->pc = 0 ;
            element->CPSR = 0;

            struct list_elem *lastElemnnn = runqueue->next;

            struct tcb *elementnn = container_of(lastElemnnn, struct tcb, rq);

            for (int i = 0; i < 13; i++){
                    ptr[i] = 0;
            }
            ptr[13] = (unsigned int)(void*)idle;
            set_registers(0,0,0);
            
            elementnn->last_ended = 1;
            dequeue_thread();

        }

void thread_create(void(* func)(void *),const void *args,unsigned int arg_size){  
    int x = 0;
    for (int c = 1; c < 33; c++)
    {
        if (tcbs[c].active == 1)
        {
            x++;
        }
    }
    if (x == 32)
    {
        kprintf("es ist kein freier Thread verfügbar\n");
    }
    else{
    int i;
   
    for (i = 1; i < 33; i++)
    {
        
        if (tcbs[i].active == 0)
        {
            tcbs[i].active = 1;
            break;
        }
    }
        
    
    for (int j=0;j<13;j++){
        tcbs[i].regs[j] = 0; 
    }
    
    tcbs[i].last_ended = 0;
    tcbs[i].id = ids + 1; 
    ids += 1; 
    void (*functionPointer2)() = &end_thread;
 
    unsigned int * thread_stack = (unsigned int*)thread_array[i-1];

    char *charPointer = (char *)args;
    for (unsigned int j = 0; j < arg_size; ++j) {
        thread_stack[j] = charPointer[j];
        tcbs[i].sp[j] =  charPointer[j];
    }
    
    tcbs[i].sp -= 8;
    if (arg_size % 8 > 0) {
        tcbs[i].sp = (unsigned int *)(8 - (arg_size % 8));
        tcbs[i].sp -= arg_size;
    }
    tcbs[i].sp = thread_stack;
    tcbs[i].regs[0] =(unsigned int ) tcbs[i].sp; 
    
    tcbs[i].CPSR = spsr();


    tcbs[i].lr = (unsigned int)functionPointer2;
   tcbs[i].pc =(unsigned int)(void *)func;
    enqueue_thread(&tcbs[i]);
    }

}

void init_idlethread(){
    tcbs[0].active = 22;         

    for (int j=0;j<13;j++){
        tcbs[0].regs[j] = 0; 
    }
    
    
    tcbs[0].last_ended = 0;                        
    tcbs[0].id = 0; 
    void (*functionPointer)() = &idle;         
    void (*functionPointer2)() = &end_thread;
    tcbs[0].sp = 0; 
    tcbs[0].lr = (unsigned int)functionPointer2;
    tcbs[0].pc = (unsigned int)functionPointer;     
    tcbs[0].CPSR = spsr();                  

}

void switch_thread(unsigned int *ptr) {
    //Fall 0: idle wird geladen
    if(runqueue == (void*)0){
        enqueue_thread(&tcbs[0]);

            for (int i = 0; i < 13; i++){
                    ptr[i] = tcbs[0].regs[0];
            }
            ptr[13] = tcbs[0].pc;
    }
    //Fall 1: idle läuft bleibt drin
    else if(runqueue != (void*)0 && runqueue->next== (void*)0){
        
        struct tcb *element = container_of(runqueue, struct tcb, rq);
        element->last_ended = 0;
    }

    //Fall 2: es gibt einen weiteren thread -> threadwechsel
    else if(runqueue != (void*)0 && runqueue->next != (void*)0){



        struct tcb *element = container_of(runqueue, struct tcb, rq);
        struct tcb *element_next = container_of(runqueue->next, struct tcb, rq);
        int x  = element->id;
        int x_next = element_next->id;
        if (x==0)                                      //Fall:   <-[i]<->[a]->...
        {
            dequeue_thread();
            enqueue_thread(&tcbs[0]);

            element = container_of(runqueue, struct tcb, rq);

            for (int i = 0; i < 13; i++){
                    ptr[i] = element->regs[i];
            }
            ptr[13] = element->pc;

            set_registers((unsigned int)element->sp,element->lr,element->CPSR);
            kprintf("\n");
            
            
        }
        else if (x!=0 && x_next == 0)                  //Fall <-[a]<->[i]->... oder <-[a]<->[i]<->[b]->...
        {
            if (element->last_ended == 0)
            {
                for (int i = 0; i < 13; i++){
                    element->regs[i]=ptr[i] ;
                }
                element->pc = ptr[13] ; 
                element->CPSR = spsr();
                element->lr = lr_usr();
                element->sp = (unsigned int *)sp_usr();

                for (int y = 1; y < 33; y++){              
                    if (tcbs[y].id == x)
                    {
                        dequeue_thread();
                        enqueue_thread(&tcbs[y]);           //thread[a] wird hinten angehangen
                        
                    }
                }
                dequeue_thread();
                enqueue_thread(&tcbs[0]);                   //idle thread ans ende
            }
            

            element = container_of(runqueue, struct tcb, rq); 

            for (int i = 0; i < 13; i++){                   //wieder thread[a] oder thread[b] wird geladen
                    ptr[i] = element->regs[i];
            }
            ptr[13] = element->pc;
            set_registers((unsigned int)element->sp,element->lr,element->CPSR);
           
            if (runqueue->next->next != (void*)0 || (element->last_ended == 1 && runqueue->next->next == (void*)0))
            {
                kprintf("\n");
            } 
            element->last_ended = 0;
        }
        else{                                           //Fall <-[a]<->[b]->...

            if (element->last_ended == 0){
                for (int i = 0; i < 13; i++){
                    element->regs[i]=ptr[i] ;
                }
                element->pc = ptr[13] ; 
                element->CPSR = spsr();
               element->lr = lr_usr();
               element->sp =(unsigned int *) sp_usr();
                
                for (int y = 1; y < 33; y++){              
                    if (tcbs[y].id == x)
                    {
                        dequeue_thread();
                        enqueue_thread(&tcbs[y]);           //thread[a] wird hinten angehangen
                        
                    }
                }
                
            }
            element->last_ended = 0;

            element = container_of(runqueue, struct tcb, rq); 

            for (int i = 0; i < 13; i++){                   //thread[b] wird geladen
                    ptr[i] = element->regs[i];
            }
            ptr[13] = element->pc;
            set_registers((unsigned int)element->sp,element->lr,element->CPSR);
            
            kprintf("\n");
            
            
        }
        
    }
    else{
        kprintf("undefinierter zustand\n");
    }
}

