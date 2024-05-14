#include <main.h>
#include <kernel/asm_hilfsfunktionen.h>
#include <stddef.h>
#include<arch/cpu/sys_timer.h>
#include <kernel/handler_asm.h>

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *mptr = (ptr);    \
        (type *)( (char *)mptr - offsetof(type,member) );})


struct list_elem  {
    struct list_elem *prev;
    struct list_elem *next;
};

struct tcb  {
    unsigned int sleepvalue;
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
struct list_elem *uartwaitqueue = (void*)0;
struct list_elem *svcwaitqueue = (void*)0;
unsigned int ids = 1;

unsigned int thread_array[] = {
    0x08FF2000, 0x08FF3000, 0x08FF4000, 0x08FF5000, 0x08FF6000,
    0x08FF7000, 0x08FF8000, 0x08FF9000, 0x08FFA000, 0x08FFB000,
    0x08FFC000, 0x08FFD000, 0x08FFE000, 0x08FFF000, 0x09000000,
    0x09001000, 0x09002000, 0x09003000, 0x09004000, 0x09005000,
    0x09006000, 0x09007000, 0x09008000, 0x09009000, 0x0900A000,
    0x0900B000, 0x0900C000, 0x0900D000, 0x0900E000, 0x0900F000,
    0x09010000, 0x09011000
};

void enqueue_thread(struct tcb *thread, int queuetype) {
    if (queuetype == 0)
    {
        if (runqueue == (void*)0) {
            thread->rq.prev = (void*)0;
            thread->rq.next = (void*)0;
            runqueue = &thread->rq;
        }
        else {
            struct list_elem *lastElem = runqueue;
            while (lastElem->next != (void*)0) {
                lastElem = lastElem->next;
                }

            lastElem->next =  &thread->rq;
            thread->rq.prev = lastElem;
            thread->rq.next = (void*)0;
        }
    }
    else if (queuetype == 1)
    {
        if (uartwaitqueue == (void*)0) {
            thread->rq.prev = (void*)0;
            thread->rq.next = (void*)0;
            uartwaitqueue = &thread->rq;
        }
        else {
            struct list_elem *lastElem = uartwaitqueue;
            while (lastElem->next != (void*)0) {
                lastElem = lastElem->next;
                }

            lastElem->next =  &thread->rq;
            thread->rq.prev = lastElem;
            thread->rq.next = (void*)0;
        }
    }
    else if (queuetype == 2)
    {
        if (svcwaitqueue == (void*)0) {
            thread->rq.prev = (void*)0;
            thread->rq.next = (void*)0;
            svcwaitqueue = &thread->rq;
        }
        else {
            struct list_elem *lastElem = svcwaitqueue;
            while (lastElem->next != (void*)0) {
                lastElem = lastElem->next;
                }

            lastElem->next =  &thread->rq;
            thread->rq.prev = lastElem;
            thread->rq.next = (void*)0;
        }
    }
    
    
}

void enqueue_thread_at_front(struct tcb *thread){
    if (runqueue == (void*)0) {
            thread->rq.prev = (void*)0;
            thread->rq.next = (void*)0;
            runqueue = &thread->rq;
    }
    else{
        struct list_elem *recentfirst = runqueue;
        recentfirst->prev = &thread->rq;
        thread->rq.prev = (void*)0;
        thread->rq.next = recentfirst;
    }
    
}
void enqueue_thread_at_second(struct tcb *thread){
    if (runqueue == (void*)0) {
            thread->rq.prev = (void*)0;
            thread->rq.next = (void*)0;
            runqueue = &thread->rq;
    }
    else{
        struct list_elem *recentfirst = runqueue->next;
        recentfirst->prev = &thread->rq;
        runqueue->next= &thread->rq;
        thread->rq.prev = runqueue;
        thread->rq.next = recentfirst;
    }
    
}

struct list_elem *dequeue_thread(int queuetype) {
    if (queuetype == 0)
    {
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
    else if (queuetype == 1)
    {
        if (uartwaitqueue == (void*)0) {
            return (void*)0;
        }

        struct list_elem *firstElem = uartwaitqueue;
        uartwaitqueue = uartwaitqueue->next;
        if (uartwaitqueue != (void*)0) {
            uartwaitqueue->prev = (void*)0;
            
        }
        return firstElem;
    }
    else if (queuetype == 2)
    {
        if (svcwaitqueue == (void*)0) {
            return (void*)0;
        }

        struct list_elem *firstElem = svcwaitqueue;
        svcwaitqueue = svcwaitqueue->next;
        if (svcwaitqueue != (void*)0) {
            svcwaitqueue->prev = (void*)0;
            
        }
        return firstElem;
    }
    else{
        kprintf("Falsche Eingabe der Queue");
        return((void*)0);
    }
}

void idle(){
    for(;;){asm("");}
}

void printrunqueue(){ //benutzten wir zum debuggen
    struct tcb *element = container_of(runqueue, struct tcb, rq);
    if (runqueue->next != (void*)0)
    {
        struct tcb *elementn = container_of(runqueue->next, struct tcb, rq);
        if (runqueue->next->next!= (void*)0)
        {
            struct tcb *elementnn = container_of(runqueue->next->next, struct tcb, rq);
            if(runqueue->next->next->next!= (void*)0)
            {
                struct tcb *elementnnn = container_of(runqueue->next->next->next, struct tcb, rq);
                if(runqueue->next->next->next->next!= (void*)0)
                {
                    
                   struct tcb *elementnnnn = container_of(runqueue->next->next->next->next, struct tcb, rq);
                    kprintf("\nptr, %u -> %u-> %u-> %u -> %u\n",element->id,elementn->id,elementnn->id,elementnnn->id,elementnnnn->id);
                }
                else{
                    kprintf("\nptr, %u -> %u-> %u-> %u\n",element->id,elementn->id,elementnn->id,elementnnn->id);
                }
            }
            else{
                kprintf("\nptr, %u -> %u-> %u\n",element->id,elementn->id,elementnn->id);
            }
            
        }
        else{
            kprintf("\nptr, %u -> %u\n",element->id,elementn->id);
        }
    }
    else{
        kprintf("runqueue: %u -> NULL\n",element->id);
    }    
}

void end_thread(unsigned int *ptr){

    struct tcb *element = container_of(runqueue, struct tcb, rq);

            for(int j = 0;j < 13;j++){
                element->regs[j] = 0 ; 
            }
            element->sleepvalue=0;
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
            dequeue_thread(0);
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
    tcbs[i].sleepvalue = 0;
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
    enqueue_thread(&tcbs[i],0);
    }

}

void init_idlethread(){

    tcbs[0].active = 1;         

    for (int j=0;j<13;j++){
        tcbs[0].regs[j] = 0; 
    }
    
    tcbs[0].sleepvalue = 0;
    tcbs[0].last_ended = 0;                        
    tcbs[0].id = 0; 
    void (*functionPointer)() = &idle;         
    void (*functionPointer2)() = &end_thread;
    tcbs[0].sp = 0; 
    tcbs[0].lr = (unsigned int)functionPointer2;
    tcbs[0].pc = (unsigned int)functionPointer;     
    tcbs[0].CPSR = spsr();                  
    enqueue_thread(&tcbs[0],0);
}
void init_mainthread(){

    tcbs[1].active = 1;         

    for (int j=0;j<13;j++){
        tcbs[1].regs[j] = 0; 
    }
    tcbs[1].sleepvalue = 0;
    tcbs[1].last_ended = 0;                        
    tcbs[1].id = 1; 
    void (*functionPointer)() = &main;         
    void (*functionPointer2)() = &end_thread;
    tcbs[1].sp = 0; 
    tcbs[1].lr = (unsigned int)functionPointer2;
    tcbs[1].pc = (unsigned int)functionPointer;     
    tcbs[1].CPSR = spsr();                  

    enqueue_thread(&tcbs[1],0);

}

void switch_thread(unsigned int *ptr,int queuetype) {
    if(runqueue != (void*)0 && runqueue->next== (void*)0){
        
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
            dequeue_thread(0);
            enqueue_thread(&tcbs[0],0);

            element = container_of(runqueue, struct tcb, rq);

            for (int i = 0; i < 13; i++){
                    ptr[i] = element->regs[i];
            }
            ptr[13] = element->pc;
            set_registers((unsigned int)element->sp,element->lr,element->CPSR);
            
            
        }
        else if (x!=0 && x_next == 0)                  //Fall <-[a]<->[i]->... oder <-[a]<->[i]<->[b]->...
        {
            if (element->last_ended == 0)              //laden des aktuellen in den tcb, wenn nicht beendet
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
                        dequeue_thread(0);
                        if (queuetype == 0)
                        {
                            enqueue_thread(&tcbs[y],0);           //thread[a] wieder in runqueue
                        }
                        else if (queuetype == 1)
                        {
                            enqueue_thread(&tcbs[y],1);         //in uart queue
                        }
                        else if (queuetype == 2)
                        {
                            enqueue_thread(&tcbs[y],2);         //in sleep queue
                        }
                        else{kprintf("Fehler bei Queue auswahl\n");
                        }
                    }
                }
                dequeue_thread(0);
                enqueue_thread(&tcbs[0],0);                   //idle thread ans ende
            }
           
            element = container_of(runqueue, struct tcb, rq); 

            for (int i = 0; i < 13; i++){                   //wieder thread[a] oder thread[b] wird geladen
                    ptr[i] = element->regs[i];
            }
            ptr[13] = element->pc;
            set_registers((unsigned int)element->sp,element->lr,element->CPSR);
        
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
                        dequeue_thread(0);
                        if (queuetype == 0)
                        {
                            enqueue_thread(&tcbs[y],0);           //thread[a] wieder in runqueue
                        }
                        else if (queuetype == 1)
                        {
                            enqueue_thread(&tcbs[y],1);         //in uart queue
                        }
                        else if (queuetype == 2)
                        {
                            enqueue_thread(&tcbs[y],2);         //in sleep queue
                        }
                        else{kprintf("Fehler bei Queue auswahl\n");
                        }
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
            
        }
        
    }
    else{
        kprintf("undefinierter zustand\n");
    }
}

void maindequeue(unsigned int *ptr){
    switch_thread(ptr,1);    //wir re-schedulen alle threads nach einem blockierenden aufruf 
}

void sleepdequeue(unsigned int *ptr){
    switch_thread(ptr,2);    //wir re-schedulen alle threads nach einem blockierenden aufruf 
    struct tcb *element = container_of(svcwaitqueue, struct tcb, rq); 
    set_compare_value(3,element->sleepvalue); 

}

int uartiswaiting(){
    if (uartwaitqueue == (void*)0)
    {
        return 0;
    }
    else{
        return 1;
    }
    
}
void sleepenqueue(unsigned int *ptr){
    struct tcb *element = container_of(svcwaitqueue, struct tcb, rq);
    for (int y = 1; y < 33; y++){              
        if (tcbs[y].id == element->id)
        {
            dequeue_thread(2);          
            enqueue_thread_at_second(&tcbs[y]);
           
        }
    }
    if (svcwaitqueue != (void*)0)
    {
        struct tcb *element = container_of(svcwaitqueue, struct tcb, rq); 
        set_compare_value(3,element->sleepvalue); 
    }
    
    switch_thread(ptr,0);       //wartender thread wird wieder gescheduled(sleep)
    
}


void uartenqueue(unsigned int *ptr){
    struct tcb *element = container_of(uartwaitqueue, struct tcb, rq);
    for (int y = 1; y < 33; y++){              
        if (tcbs[y].id == element->id)
        {
            dequeue_thread(1);
            enqueue_thread_at_second(&tcbs[y]);
        }
    }
    switch_thread(ptr,0);          //wartender thread wird wieder gescheduled(uart)
}

void setcomparevalue_tcb(unsigned int x){
    struct tcb *element = container_of(runqueue, struct tcb, rq);
    element->sleepvalue = x;
}