all: floppya.img  loadFile

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

kernel: kernel_asm.o kernel.o
	ld86 -o kernel -d kernel.o kernel_asm.o

shell.o: shell.c
	bcc -ansi -c -o shell.o shell.c

lib.o: lib.asm
	as86 lib.asm -o lib.o

shell: lib.o shell.o
	ld86 -o shell -d shell.o lib.o

floppya.img: kernel shell
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
	dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
	./loadFile message.txt
	./loadFile bigmess.txt
	./loadFile tstpr2
	./loadFile shell


loadFile.o: loadFile.c
	gcc -c loadFile.c -o loadFile.o

loadFile: loadFile.o
	gcc loadFile.o -o loadFile

clean:
	rm -f kernel shell floppya.img *.o
