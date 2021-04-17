bcc -ansi -c -o bin/hello src/hello.c
ld86 -o bin/hello_linked -d bin/hello output/lib_asm.o
