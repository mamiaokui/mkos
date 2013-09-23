#ifndef ASMPACK_H
#define ASMPACK_H
extern "C" void asmHlt(void);
extern "C" void asmWriteMemory8(int address, int value);
extern "C" void asmCli(void);
extern "C" void asmStiHlt(void);
extern "C" void asmOut8(int port, int data);
extern "C" unsigned char asmIn8(int port);
extern "C" int asmLoadEflags(void);
extern "C" void asmStoreEflags(int eflags);
extern "C" int asmLoadCr0(void);
extern "C" void asmStoreCr0(int cr0);
extern "C" void asmLoadGDTR(int size, int address);
extern "C" void asmLoadIDTR(int size, int address);
extern "C" void asmSti();
extern "C" void asmLoadTR(int);
extern "C" void asmTaskSwitch4();
extern "C" void asmAddCount();
extern "C" char asmGlobalFont;
#endif
