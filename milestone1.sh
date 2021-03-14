nasm ./asm/bootloader.asm -o ./output/bootloader
dd if=./output/bootloader of=./output/system.img bs=512 count=1 conv=notrunc
bcc -ansi -c -o ./output/kernel.o ./src/kernel.c
bcc -ansi -c -o ./output/utilities.o ./src/utilities.c
bcc -ansi -c -o ./output/shell.o ./src/shell.c
nasm -f as86 ./asm/kernel.asm -o ./output/kernel_asm.o
ld86 -o ./output/kernel -d ./output/kernel.o ./output/kernel_asm.o ./output/utilities.o ./output/shell.o
dd if=./output/kernel of=./output/system.img bs=512 conv=notrunc seek=1
