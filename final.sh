rm output/system.img

nasm ./asm/bootloader.asm -o ./output/bootloader
dd if=./output/bootloader of=./output/system.img bs=512 count=1 conv=notrunc
bcc -ansi -c -o ./output/kernel.o ./src/kernel.c
bcc -ansi -c -o ./output/utilities.o ./src/utilities.c
bcc -ansi -c -o ./output/shell.o ./src/shell.c
nasm -f as86 ./asm/kernel.asm -o ./output/kernel_asm.o
ld86 -o ./output/kernel -d ./output/kernel.o ./output/kernel_asm.o ./output/utilities.o ./output/shell.o
dd if=./output/kernel of=./output/system.img bs=512 conv=notrunc seek=1

dd if=/dev/zero of=./output/map.img bs=512 count=1
dd if=/dev/zero of=./output/files.img bs=512 count=2
dd if=/dev/zero of=./output/sectors.img bs=512 count=1

python3 script/maphexedit.py

dd if=./output/map.img of=./output/system.img bs=512 count=1 seek=256 conv=notrunc
dd if=./output/files.img of=./output/system.img bs=512 count=2 seek=257 conv=notrunc
dd if=./output/sectors.img of=./output/system.img bs=512 count=1 seek=259 conv=notrunc

python3 script/loadfile.py
