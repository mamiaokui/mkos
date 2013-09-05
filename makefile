IPL.o : IPL.nas
	nasm IPL.nas -o IPL.o

AsmHead.o: AsmHead.nas
	nasm AsmHead.nas -o AsmHead.o

RESB.o: RESB.nas
	nasm RESB.nas -o RESB.o

BootProgram.o: BootProgram.c
	gcc -c BootProgram.c -o BootProgram.o

AsmFunction.o: AsmFunction.nas
	nasm -f elf  AsmFunction.nas -o AsmFunction.o

BootProgramLink.o: AsmFunction.o BootProgram.o
	ld -m elf_i386 -Ttext 0x290000  BootProgram.o AsmFunction.o -o BootProgramLink.o

OS.img: IPL.o AsmHead.o BootProgramLink.o RESB.o
	cat IPL.o AsmHead.o BootProgramLink.o RESB.o > OS.img

all: OS.img
	@echo ok

clean:
	rm *.o
	rm *.img