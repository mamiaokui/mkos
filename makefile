IPL.o : IPL.asm
	nasm IPL.asm -o IPL.o

AsmHead.o: AsmHead.asm
	nasm AsmHead.asm -o AsmHead.o

RESB.o: RESB.asm
	nasm RESB.asm -o RESB.o

BootProgram.o: BootProgram.c
	gcc -c -O0 -m32 -fno-stack-protector BootProgram.c -o BootProgram.o

AsmFunction.o: AsmFunction.asm
	nasm -f elf  AsmFunction.asm -o AsmFunction.o

BootProgramLink.o: AsmFunction.o BootProgram.o
	ld -m elf_i386 -Ttext 0x290000  BootProgram.o AsmFunction.o -o BootProgramLink.o

OS.img: IPL.o AsmHead.o BootProgramLink.o RESB.o
	cat IPL.o AsmHead.o BootProgramLink.o RESB.o > OS.img

all: OS.img
	@echo ok

clean:
	rm -f *.o
	rm -f *.img