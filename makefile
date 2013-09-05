ipl.o : ipl.nas
	nasm ipl.nas -o ipl.o

asmhead.o: asmhead.nas
	nasm asmhead.nas -o asmhead.o

resb.o: resb.nas
	nasm resb.nas -o resb.o

bootpack.o: bootpack.c
	gcc -c -m32 bootpack.c -o bootpack.o

naskfunc.o: naskfunc.nas
	nasm -f elf  naskfunc.nas -o naskfunc.o

bootpacklink.o: naskfunc.o bootpack.o
	ld -m elf_i386 -Ttext 0x290000  bootpack.o naskfunc.o -o bootpacklink.o

os.img: ipl.o asmhead.o bootpack.o resb.o bootpacklink.o
	cat ipl.o asmhead.o bootpacklink.o resb.o> os.img

all: os.img
	@echo ok

clean:
	rm *.o
	rm *.img