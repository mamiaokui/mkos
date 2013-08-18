ipl.o : ipl.nas
	@echo nasm ipl.nas -o ipl.o
	nasm ipl.nas -o ipl.o

haribote.o: haribote.nas
	@echo nasm haribote.nas -o haribote.o
	nasm haribote.nas -o haribote.o

os.img: ipl.o haribote.o
	cat ipl.o haribote.o > os.img

all: os.img
	@echo ok

clean:
	rm *.o
	rm *.img