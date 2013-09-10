extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);
extern void asmLog(int value);
extern void asmLoadGDTR(int size, int address);
extern void asmLoadIDTR(int size, int address);
extern char asmGlobalFont;

#define BOOTINFO_ADDRESS 0x0ff0 //look for the head of AsmHead.asm for reason.
