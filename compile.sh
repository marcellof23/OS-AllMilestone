nasm ./asm/bootloader.asm -o ./output/bootloader

dd if=./output/bootloader of=./output/system.img bs=512 count=1 conv=notrunc
bcc -ansi -c -o ./output/kernel.o ./src/kernel.c

bcc -ansi -c -o ./output/math.o ./src/module/math.c
bcc -ansi -c -o ./output/stringutil.o ./src/module/stringutil.c
bcc -ansi -c -o ./output/fileIO.o ./src/module/fileIO.c

bcc -ansi -c -o ./output/shell.o ./src/shell.c

nasm -f as86 ./asm/kernel.asm -o ./output/kernel_asm.o
ld86 -o ./output/kernel -d ./output/kernel.o ./output/kernel_asm.o ./output/math.o ./output/stringutil.o ./output/fileIO.o

nasm -f as86 ./asm/lib.asm -o ./output/lib_asm.o
# ld86 -o ./output/lib -d ./output/lib_asm.o

ld86 -o ./bin/shell -d ./output/shell.o ./output/lib_asm.o ./output/math.o ./output/stringutil.o

./compile_lib.sh

dd if=./output/kernel of=./output/system.img bs=512 conv=notrunc seek=1