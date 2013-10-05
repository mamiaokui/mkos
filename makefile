IPL.o : IPL.asm
	nasm IPL.asm -o IPL.o

AsmHead.o: AsmHead.asm
	nasm -g AsmHead.asm -o AsmHead.o

RESB.o: RESB.asm
	nasm RESB.asm -o RESB.o

BootProgram.o: BootProgram.cpp
	gcc -c -O0 -m32 BootProgram.cpp -o BootProgram.o

AsmPack.o: AsmPack.asm
	nasm -f elf  AsmPack.asm -o AsmPack.o

FontData.o: FontData.asm
	nasm  -f elf FontData.asm -o FontData.o

GdtIdt.o: GdtIdt.h GdtIdt.cpp
	gcc -c -O0 -m32  GdtIdt.cpp -o GdtIdt.o

PaintPack.o: PaintPack.h PaintPack.cpp
	gcc -c -O0 -m32  PaintPack.cpp -o PaintPack.o

Utils.o: Utils.h Utils.cpp
	gcc -c -O0 -m32  Utils.cpp -o Utils.o

Platform.o: Platform.h Platform.cpp
	gcc -c -O0 -m32  Platform.cpp -o Platform.o

InterruptionBuffer.o: InterruptionBuffer.h InterruptionBuffer.cpp
	gcc -c -O0 -m32  InterruptionBuffer.cpp -o InterruptionBuffer.o

BootProgramStart.o: BootProgramStart.asm
	nasm -f elf BootProgramStart.asm -o BootProgramStart.o

BootProgramLink.o: AsmPack.o BootProgram.o BootProgramStart.o FontData.o GdtIdt.o PaintPack.o Utils.o Platform.o InterruptionBuffer.o
	ld -m elf_i386 -Ttext 0x200000  BootProgramStart.o BootProgram.o AsmPack.o FontData.o GdtIdt.o PaintPack.o Utils.o Platform.o InterruptionBuffer.o -o BootProgramLink.o

OS.img: IPL.o AsmHead.o BootProgramLink.o RESB.o
	cat IPL.o AsmHead.o BootProgramLink.o RESB.o > OS.img
	@bash check.sh

all: OS.img makefile
	@ls -l OS.img	

clean:
	rm -f *.o
	rm -f *.img

run: clean all makefile 
	qemu OS.img