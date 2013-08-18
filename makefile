ipl.o : ipl.nas
	@echo nasm ipl.nas -o ipl.o
	nasm ipl.nas -o ipl.o

asmhead.o: asmhead.nas
	@echo nasm asmhead.nas -o asmhead.o
	nasm asmhead.nas -o asmhead.o

os.img: ipl.o asmhead.o
	cat ipl.o asmhead.o > os.img

all: os.img
	@echo ok

clean:
	rm *.o
	rm *.img