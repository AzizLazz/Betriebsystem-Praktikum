#include <config.h>
#include <arch/cpu/mmu_asm.h>
#include <kernel.h>
#include <kernel/kprintf.h>

#define MB (1048576)
#define KB (4096)

#define USERDATA_BASE_ADRESS (6291456)

#define UARTBYTE (1008)
#define SYSTIMERYBTE (1010)

#define THREADS_NB (32)
#define L1_SIZE (4096)
#define L2_SIZE (256)

#define L2_BASIS_ADDR (99614720)

#define PROCESS_THREAD_START (12582912)
#define PROCESS_BASIS_ADDR (9437184)
#define PROCESS_SIZE (4194304)
#define PROCESS_STACK_BASE_1 (3149824)  //PROCESS_BASIS_ADDR + PROCESS_STACK_BASE_1 = letzte adresse vor guardpage
#define PROCESS_STACK_BASE_2 (3219456)  //PROCESS_BASIS_ADDR + i * PROCESS_SIZE für die anderen stacks
#define PROCESS_STACK_BASE_3 (3289088)
#define PROCESS_STACK_BASE_4 (3358720)  
#define PROCESS_NR (8)

#define XN_PXN (17)
#define L2EintragmitPXN (5)
#define L2EintragohnePXN (1)
#define PXN (1)
#define XN_L2 (1)

#define KERNEL_RW (1026)
#define KERNEL_RW_USER_R (2050)
#define KERNEL_R (33794)
#define READ_ONLY (35842)
#define VOLLZUGRIFF_L2 (50)
#define VOLLZUGRIFF (3074)
#define NO_ACESS (2)


__attribute__((section(".exeptionstacks"))) char MyArray[MB];
__attribute__((section(".threadstacks"))) char MyArray2[THREADS_NB * MB];


unsigned int tabL1[L1_SIZE] __attribute__((aligned(0x4000))) ; 
unsigned int threadL2tables[THREADS_NB][L2_SIZE]; 
unsigned int processL2tables[PROCESS_NR][L2_SIZE]; 

void switchMapping(int processnr){
    if (processnr == -1)    //mapped die datasektion auf sich selbst, zum kopieren
    {
        tabL1[6] = (unsigned int)MB * 6 | (READ_ONLY + XN_PXN);         
        tabL1[7] = (unsigned int)MB * 7 | (READ_ONLY + XN_PXN);   
        tabL1[8] = (unsigned int)MB * 8 | (READ_ONLY + XN_PXN);   

    }
    else{ //mapped die data section auf den richtigen Prozess
        tabL1[6] = (unsigned int)MB * (unsigned int)(9+ (processnr*4)) | (READ_ONLY + XN_PXN);     
        tabL1[7] = (unsigned int)MB * (unsigned int)(10+(processnr*4)) | (VOLLZUGRIFF + XN_PXN);
        tabL1[8] = (unsigned int)MB * (unsigned int)(11+(processnr*4)) | (VOLLZUGRIFF + XN_PXN);
    }
    
}

void initMapping(int id, int stacknr, int process){
    //mapped die stacks des Prozesses auf die pyhischen Stacks
    if (stacknr == 1)
    {
        for (int i = 1,j=0; i < 17; i++,j++){
            processL2tables[process][i] = (THREADS_STACK_BASE - THREADS_STACK_SIZE - (unsigned int)(MB * id)+ (unsigned int)(j*KB)) | (VOLLZUGRIFF_L2 + XN_L2);
        }
       
    }
    else if (stacknr == 2)
    {
        for (int i = 18,j=0; i < 34; i++,j++)
        {
            processL2tables[process][i] = (THREADS_STACK_BASE - THREADS_STACK_SIZE - (unsigned int)(MB * id)+ (unsigned int)(j*KB)) | (VOLLZUGRIFF_L2 + XN_L2);
        }
    }
    else if (stacknr == 3)
    {
        for (int i = 35,j=0; i < 51; i++,j++)
        {
            processL2tables[process][i] = (THREADS_STACK_BASE - THREADS_STACK_SIZE - (unsigned int)(MB * id)+ (unsigned int)(j*KB)) | (VOLLZUGRIFF_L2 + XN_L2);
        }
    }
    //mapping von rodata und data auf prozess
    tabL1[6] = (unsigned int)MB * (unsigned int)(9+ (process*4)) | (READ_ONLY + XN_PXN);     
    tabL1[7] = (unsigned int)MB * (unsigned int)(10+(process*4)) | (VOLLZUGRIFF + XN_PXN);   
    tabL1[8] = (unsigned int)MB * (unsigned int)(11+(process*4)) | (VOLLZUGRIFF + XN_PXN);   
    
}

void memcopy(void *ziel, void *quelle, int Anzahl)
{  
    char *qpointer = (char *)quelle;  
    char *zpointer = (char *)ziel;  

    for (int i=0; i<Anzahl; i++)  {
        zpointer[i] = qpointer[i];  
    }
}  


void load_user_data(int prcnr){
        memcopy((void *)((unsigned int)PROCESS_BASIS_ADDR+(unsigned int)(PROCESS_SIZE * prcnr)), (void *)(unsigned int)(USERDATA_BASE_ADRESS), 3145728); //3MB = Groeße der datasektion 
    
}

void eintraegeausserhalb(){
    for (int i = 128; i < L1_SIZE; i++) //faulteintrage
    {
        tabL1[i] = 0;
    }

    tabL1[UARTBYTE] = (unsigned int)((unsigned int)MB * UARTBYTE  ) | (KERNEL_RW + XN_PXN); //uart 
    tabL1[SYSTIMERYBTE] = (unsigned int)((unsigned int)MB * SYSTIMERYBTE) | (KERNEL_RW + XN_PXN) ; //interrupt controller und systimer
}

void sektioneintraege(){

    tabL1[0] = (NO_ACESS + XN_PXN) ;                            //bootsection   
    tabL1[1] =  ((unsigned int)MB | KERNEL_R);                  //kernelcode    
    tabL1[2] = ((unsigned int)MB * 2) | (KERNEL_R + XN_PXN);    //kernelrodata  
    tabL1[3] = (unsigned int)MB * 3 | (KERNEL_RW + XN_PXN);     //kerneldata    
    tabL1[4] = (unsigned int)MB * 4 | (KERNEL_RW + XN_PXN);     //kerneldata

    tabL1[5] = (unsigned int)MB * 5 | (READ_ONLY + PXN);        //usercode      
    tabL1[6] = (unsigned int)MB * 6 | (READ_ONLY + XN_PXN);     //userrodata    
    tabL1[7] = (unsigned int)MB * 7 | (READ_ONLY + XN_PXN);   //userdata      
    tabL1[8] = (unsigned int)MB * 8 | (READ_ONLY + XN_PXN);   //userdata

//process einträge
    for (int i = 0,j = 9; i < 8; i++,j+=4)
    {
        tabL1[j]   = (unsigned int)MB * j   | (KERNEL_RW_USER_R + XN_PXN);     //userrodata 
        tabL1[j+1] = (unsigned int)MB * (j+1) | (VOLLZUGRIFF + XN_PXN);   //userdata  
        tabL1[j+2] = (unsigned int)MB * (j+2) | (VOLLZUGRIFF + XN_PXN);   //userdata
        tabL1[j+3] = ((unsigned int)&processL2tables[i]) | L2EintragohnePXN;   //prozess thread L2

    }

    for (int i = 45; i < 95; i++) //sektionseinträge für freespace = faulteinträge
        {
            tabL1[i] = 0;
        } 

    for (int i =95,j=0; i < 127; i++,j++) //l2 pointer einträge 
    {
        tabL1[i] = ((unsigned int)&threadL2tables[j]) | L2EintragmitPXN;
    }
    tabL1[127] =  (unsigned int)((unsigned int)MB * 127) | (KERNEL_RW + XN_PXN) ;
}    

void threadL2eintraege(){ 
    for (int j = 0; j < THREADS_NB; j++)
    {
        for (int i = 239; i < L2_SIZE; i++) //256KB-239KB = 17KB = stack mit guardpage
        {
            threadL2tables[j][i] = (unsigned int)((unsigned int)L2_BASIS_ADDR +(unsigned int)(j*MB) + (unsigned int)(i*KB)) | (VOLLZUGRIFF_L2 + XN_L2) ; 
        }
    }
    for (int j = 0; j < THREADS_NB; j++)
    {
        for (int i = 0; i < 239; i++) //initialisieren der guardpages
        {
                threadL2tables[j][i] = (unsigned int)((unsigned int)L2_BASIS_ADDR +(unsigned int)(j*MB) + (unsigned int)(i*KB)) | (NO_ACESS + XN_L2) ; 
        }
    }
}

void processL2eintraege(){  
    for (int j = 0; j < PROCESS_NR; j++)    //stacks size: (4*1024) * 16 = 16KB
    {
        for (int i = 0; i < 68; i++)    //68 = 17 * 4
        {
            processL2tables[j][i] = (unsigned int)((unsigned int)PROCESS_THREAD_START +(unsigned int)(j*PROCESS_SIZE) + (unsigned int)(i*KB)) | (VOLLZUGRIFF_L2 + XN_L2) ; 
        }
    }
    for (int j = 0; j < PROCESS_NR; j++)//nicht zugreifbar(auserhalb der stacks)
    {
        for (int i = 68; i < L2_SIZE; i++) 
        {
                processL2tables[j][i] = (unsigned int)((unsigned int)PROCESS_THREAD_START+(unsigned int)(j*PROCESS_SIZE) + (unsigned int)(i*KB)) | (NO_ACESS + XN_L2) ; 
        }
    }   
    for (int j = 0; j < PROCESS_NR; j++)    //guardpages innerhalb der process stacks
    {
        processL2tables[j][0]  = (unsigned int)((unsigned int)PROCESS_THREAD_START+(unsigned int)(j*PROCESS_SIZE))                      | (NO_ACESS + XN_L2) ; 
        processL2tables[j][17] = (unsigned int)((unsigned int)PROCESS_THREAD_START+(unsigned int)(j*PROCESS_SIZE)+(unsigned int)(17*KB))| (NO_ACESS + XN_L2) ; 
        processL2tables[j][34] = (unsigned int)((unsigned int)PROCESS_THREAD_START+(unsigned int)(j*PROCESS_SIZE)+(unsigned int)(34*KB))| (NO_ACESS + XN_L2) ; 
        processL2tables[j][51] = (unsigned int)((unsigned int)PROCESS_THREAD_START+(unsigned int)(j*PROCESS_SIZE)+(unsigned int)(51*KB))| (NO_ACESS + XN_L2) ; 
    
    }
}

void mmu_enable() {
    enable_SCTLR();
}


void mmu_vorbi() {

    sektioneintraege();
    threadL2eintraege();
    processL2eintraege();
    eintraegeausserhalb();

    //  Zeiger auf L1-Tabelle in TTBR0 hinterlegen (TTBR1 wird nicht benötigt)
    set_TTBR0((unsigned int)(&tabL1)) ;

    //  Domain Access Control setzen (Domain 0 → client mode)
    set_DACR() ;

    //  32-Bit Umsetzung (Short) in CP15 TTB Control Register (TTBCR) aktivieren.
    enable_32bit_TTBCR();

    disable_caches();
    mmu_enable();
}



/*
Speicheraufbau: 

0x00000000 : Bootfiles-----------------Zugriff: 000----XN,PXN
0x00100000 : Kernel-Code---------------Zugriff: 101----
0x00200000 : KernelroData--------------Zugriff: 101----XN,PXN
0x00300000 : KernelData----------------Zugriff: 001----XN,PXN
0x00400000 : KernelData----------------Zugriff: 001----XN,PXN
0x00500000 : UserCode------------------Zugriff: 111----PXN
0x00600000 : UserroData----------------Zugriff: 111----XN,PXN
0x00700000 : UserData------------------Zugriff: 011----XN,PXN
0x00800000 : UserData------------------Zugriff: 011----XN,PXN

PROZESS-AUFBAU:
0x00900000 : prozess1rodata------------------ L1 tabelle
0x00a00000 : prozess1data-------------------- L1 tabelle
0x00b00000 : prozess1data-------------------- L1 tabelle
0x00c00000 : prozess1threads----------------- L2 tabelle

0x00c00000 --guardpage
0x00c01000 --stack 1
0x00c02000
0x00c03000
0x00c04000
0x00c05000
0x00c06000
0x00c07000
0x00c08000
0x00c09000
0x00c0a000
0x00c0b000
0x00c0c000
0x00c0d000
0x00c0e000
0x00c0f000
0x00c10000
0x00c11000 --guardpage
0x00c12000 --stack2
0x00c13000
0x00c14000
0x00c15000
0x00c16000
0x00c17000
0x00c18000
0x00c19000
0x00c1a000
0x00c1b000
0x00c1c000
0x00c1d000
0x00c1e000
0x00c1f000
0x00c20000
0x00c21000
0x00c22000 --guardpage
0x00c23000 --stack3
0x00c24000
0x00c25000
0x00c26000
0x00c27000
0x00c28000
0x00c29000
0x00c2a000
0x00c2b000
0x00c2c000
0x00c2d000
0x00c2e000
0x00c2f000
0x00c30000
0x00c31000
0x00c32000
0x00c33000 --guardpage
0x00c34000 --stack4
.
.
.

Nach den 8 Prozessen:

"Free Space"---------------------------faulteinträge
0x05F00000 : ThreadStacks--------------Zugriff: 011----XN

0x07F00000 : Exceptionstacks-----------Zugriff: 001----XN, PXN

perepherie: kernel rw xn, pxn

000 No access No access kein Zugriff
001 Read/write No access System-Zugriff
101 Read-only No access System-Nur-Lesen
111 Read-only Read-only Beide-Nur-Lesen
010 Read/write Read-only Nur-Lesen
011 Read/write Read/write Vollzugriff

*/
