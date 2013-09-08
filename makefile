IPL.o : IPL.asm
	nasm IPL.asm -o IPL.o

AsmHead.o: AsmHead.asm
	nasm -g AsmHead.asm -o AsmHead.o

RESB.o: RESB.asm
	nasm RESB.asm -o RESB.o

BootProgram.o: BootProgram.c
	gcc -c -g -O0 -m32 -fno-stack-protector BootProgram.c -o BootProgram.o

AsmFunction.o: AsmFunction.asm
	nasm -f elf  AsmFunction.asm -o AsmFunction.o

FontData.o: FontData.asm
	nasm  FontData.asm -o FontData.o

BootProgramStart.o: BootProgramStart.asm
	nasm -f elf BootProgramStart.asm -o BootProgramStart.o

BootProgramLink.o: AsmFunction.o BootProgram.o BootProgramStart.o
	ld -m elf_i386 -Ttext 0x00280000  BootProgramStart.o BootProgram.o AsmFunction.o -o BootProgramLink.o

OS.img: IPL.o AsmHead.o FontData.o BootProgramLink.o RESB.o
	cat IPL.o AsmHead.o FontData.o BootProgramLink.o RESB.o > OS.img

all: OS.img
	@echo ok

clean:
	rm -f *.o
	rm -f *.img