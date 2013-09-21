IPL.o : IPL.asm
	nasm IPL.asm -o IPL.o

AsmToCpp.o: AsmToCpp.asm
	nasm -g AsmToCpp.asm -o AsmToCpp.o

RESB.o: RESB.asm
	nasm RESB.asm -o RESB.o

BootProgram.o: BootProgram.cpp
	gcc -c -O0 -m32 -fno-stack-protector BootProgram.cpp -o BootProgram.o

AsmTools.o: AsmTools.asm
	nasm -f elf  AsmTools.asm -o AsmTools.o

FontData.o: FontData.asm
	nasm  -f elf FontData.asm -o FontData.o

GdtIdt.o: GdtIdt.h GdtIdt.cpp
	gcc -c -O0 -m32 -fno-stack-protector GdtIdt.cpp -o GdtIdt.o

PaintPack.o: PaintPack.h PaintPack.cpp
	gcc -c -O0 -m32 -fno-stack-protector PaintPack.cpp -o PaintPack.o

Utils.o: Utils.h Utils.cpp
	gcc -c -O0 -m32 -fno-stack-protector Utils.cpp -o Utils.o

Platform.o: Platform.h Platform.cpp
	gcc -c -O0 -m32 -fno-stack-protector Platform.cpp -o Platform.o

InterruptionBuffer.o: InterruptionBuffer.h InterruptionBuffer.cpp
	gcc -c -O0 -m32 -fno-stack-protector InterruptionBuffer.cpp -o InterruptionBuffer.o

MemoryManager.o: MemoryManager.h MemoryManager.cpp
	gcc -c -O0 -m32 -fno-stack-protector MemoryManager.cpp -o MemoryManager.o

LayerManager.o: LayerManager.h LayerManager.cpp
	gcc -c -O0 -m32 -fno-stack-protector LayerManager.cpp -o LayerManager.o

StartupManager.o: StartupManager.h StartupManager.cpp
	gcc -c -O0 -m32 -fno-stack-protector StartupManager.cpp -o StartupManager.o

Timer.o: Timer.h Timer.cpp
	gcc -c -O0 -m32 -fno-stack-protector Timer.cpp -o Timer.o


BootProgramStart.o: BootProgramStart.asm
	nasm -f elf BootProgramStart.asm -o BootProgramStart.o

BootProgramLink.o: AsmTools.o BootProgram.o BootProgramStart.o FontData.o GdtIdt.o PaintPack.o Utils.o Platform.o InterruptionBuffer.o MemoryManager.o LayerManager.o StartupManager.o Timer.o
	ld -m elf_i386 -Ttext 0x200000  BootProgramStart.o BootProgram.o AsmTools.o FontData.o GdtIdt.o PaintPack.o Utils.o Platform.o InterruptionBuffer.o MemoryManager.o LayerManager.o StartupManager.o Timer.o -o BootProgramLink.o

OS.img: IPL.o AsmToCpp.o BootProgramLink.o RESB.o
	cat IPL.o AsmToCpp.o BootProgramLink.o RESB.o > OS.img
	@bash check.sh

all: OS.img makefile
	@ls -l OS.img	

clean:
	rm -f *.o
	rm -f *.img

run: clean all makefile 
	qemu -m 256 OS.img

test: MemoryManager.cpp test.cpp LayerManager.cpp
	g++ -g -O0  -DMKDEBUG=1 MemoryManager.cpp LayerManager.cpp test.cpp  -o test.o
