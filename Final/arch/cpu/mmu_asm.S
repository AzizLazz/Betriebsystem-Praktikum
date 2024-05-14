.global read_TTBR0
.global set_TTBR0
.global read_DACR
.global set_DACR
.global enable_SCTLR
.global disable_SCTLR
.global enable_32bit_TTBCR
.global disable_caches
.global tlbhandling



read_TTBR0:
    MRC p15, 0, r0, c2, c0, 0
    bx lr 

set_TTBR0:
    MCR p15, 0, r0, c2, c0, 0     //Set TTBR0 with the value in r0
    bx lr                      

read_DACR:
    MRC p15, 0, r0, c3, c0, 0    // Read DACR into r0
    bx lr                       

set_DACR:
    LDR r0, =0x55555555           
    MCR p15, 0, r0, c3, c0, 0     //Set DACR with the value in r0
    bx lr                       

enable_SCTLR:
    MRC p15, 0, r0, c1, c0, 0   
    ORR r0, r0, #1                
    MCR p15, 0, r0, c1, c0, 0     
    bx lr                       
                      
enable_32bit_TTBCR:
    MOV r0, #0   
    MCR p15, 0, r0, c2, c0, 2  
    bx lr                      

disable_caches:
    MRC p15, 0, r0, c1, c0, 0  
    BIC r0, r0, #0b10      
    BIC r0, r0, #0b100000000000     
    MCR p15, 0, r0, c1, c0, 0  
    bx lr                      

tlbhandling:
    //MCR p15, 0, r0, c8, c6, 0 //page1740 unten: inavlidates all TLB (für kontextwechsel (seite 31)) data tlb
    MCR p15, 0, r0, c8, c7, 0   //instr tlb
    bx lr 
