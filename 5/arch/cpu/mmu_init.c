#include <config.h>
#include <arch/cpu/mmu_asm.h>

#define MB (1048576)

#define UARTBYTE (1008)
#define SYSTIMERYBTE (1010)

#define THREADS_NB (32)
#define L1_SIZE (4096)
#define L2_SIZE (256)

#define L2_BASIS_ADDR (99614720)

#define XN_PXN (17)
#define L2EintragmitPXN (5)
#define PXN (1)
#define XN_L2 (1)

#define KERNEL_RW (1026)
#define KERNEL_R (33794)
#define READ_ONLY (35842)
#define VOLLZUGRIFF_L2 (50)
#define VOLLZUGRIFF (3074)
#define NO_ACESS (2)


__attribute__((section(".exeptionstacks"))) char MyArray[MB];
__attribute__((section(".threadstacks"))) char MyArray2[THREADS_NB * MB];


unsigned int tabL1[L1_SIZE] __attribute__((aligned(0x4000))) ; 
unsigned int threadL2tables[THREADS_NB][L2_SIZE]; 
unsigned int uartL2table[L2_SIZE]; 
unsigned int systimerL2table[L2_SIZE]; 


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
    tabL1[7] = (unsigned int)MB * 7 | (VOLLZUGRIFF + XN_PXN);   //userdata      
    tabL1[8] = (unsigned int)MB * 8 | (VOLLZUGRIFF + XN_PXN);   //userdata


    for (int i = 9; i < 95; i++) //sektionseinträge für freespace = faulteinträge
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
        for (int i = 0; i < L2_SIZE; i++) 
        {
            threadL2tables[j][i] = (unsigned int)((unsigned int)L2_BASIS_ADDR +(unsigned int)(j*1024*1024) + (unsigned int)(i*4*1024)) | (VOLLZUGRIFF_L2 + XN_L2) ; 
        }
    }

    for (int i = 0; i < THREADS_NB; i++) //initialisieren der guardpages
    {
            threadL2tables[i][0] = (unsigned int)((unsigned int)L2_BASIS_ADDR +(unsigned int)(i*1024*1024) ) | (NO_ACESS + XN_L2) ; 
    }
    
}


void mmu_enable() {
enable_SCTLR();
}


void mmu_vorbi() {

    sektioneintraege();
    threadL2eintraege();
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
