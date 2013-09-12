#ifndef ASMPACK_H
#define ASMPACK_H
extern void asmHlt(void);
extern void asmWriteMemory8(int address, int value);
extern void asmCli(void);
extern void asmOut8(int port, int data);
extern void asmIn8(int port);
extern int asmLoadEflags(void);
extern void asmStoreEflags(int eflags);
extern void asmLog(int value);
extern void asmLoadGDTR(int size, int address);
extern void asmLoadIDTR(int size, int address);
extern void asmSti();
extern char asmGlobalFont;


#endif
