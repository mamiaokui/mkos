IPL.o : IPL.asm
	nasm IPL.asm -o IPL.o

AsmHead.o: AsmHead.asm
	nasm -g AsmHead.asm -o AsmHead.o

RESB.o: RESB.asm
	nasm RESB.asm -o RESB.o

BootProgram.o: BootProgram.c
	gcc -c -O0 -m32 -fno-stack-protector BootProgram.c -o BootProgram.o

AsmFunction.o: AsmFunction.asm
	nasm -f elf  AsmFunction.asm -o AsmFunction.o

FontData.o: FontData.asm
	nasm  -f elf FontData.asm -o FontData.o

GdtIdt.o: GdtIdt.h GdtIdt.c
	gcc -c -O0 -m32 -fno-stack-protector GdtIdt.c -o GdtIdt.o

PaintPack.o: PaintPack.h PaintPack.c
	gcc -c -O0 -m32 -fno-stack-protector PaintPack.c -o PaintPack.o

Utils.o: Utils.h Utils.c
	gcc -c -O0 -m32 -fno-stack-protector Utils.c -o Utils.o

BootProgramStart.o: BootProgramStart.asm
	nasm -f elf BootProgramStart.asm -o BootProgramStart.o

BootProgramLink.o: AsmFunction.o BootProgram.o BootProgramStart.o FontData.o GdtIdt.o PaintPack.o Utils.o
	ld -m elf_i386 -Ttext 0x00280000  BootProgramStart.o BootProgram.o AsmFunction.o FontData.o GdtIdt.o PaintPack.o Utils.o -o BootProgramLink.o

OS.img: IPL.o AsmHead.o BootProgramLink.o RESB.o
	cat IPL.o AsmHead.o BootProgramLink.o RESB.o > OS.img
	@bash check.sh

all: OS.img makefile
	@ls -l OS.img	

clean:
	rm -f *.o
	rm -f *.img

run: all makefile
	qemu OS.img