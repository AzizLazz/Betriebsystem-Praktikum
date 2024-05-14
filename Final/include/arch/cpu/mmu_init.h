#ifndef MMU_H
#define MMU_H


extern char MyArray[20480];
extern char MyArray2[33554432];

extern unsigned int tabL1[4096] __attribute__((aligned(0x4000)));
extern unsigned int threadL2tables[32][256];
extern unsigned int uartL2table[256];
extern unsigned int systimerL2table[256];

void memcopy(void *ziel, void *quelle, int Anzahl);
void load_user_data(int prcnr);

void switchMapping(int processnr);
void initMapping(int id, int stacknr, int process);

void processL2eintraege();
void eintraegeausserhalb();
void sektioneintraege();
void threadL2eintraege();
void mmu_enable();
void mmu_vorbi();

#endif // MMU_H
