all: floppya.img
kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c
kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o
kernel: kernel_asm.o kernel.o
	ld86 -o kernel -d kernel.o kernel_asm.o
floppya.img: kernel
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
clean:
	rm -f kernel floppya.img *.o
