bcc -ansi -c -o ./output/ln.o ./src/lib/ln.c
bcc -ansi -c -o ./output/cat.o ./src/lib/cat.c
bcc -ansi -c -o ./output/cp.o ./src/lib/cp.c
bcc -ansi -c -o ./output/mkdir.o ./src/lib/mkdir.c
bcc -ansi -c -o ./output/mv.o ./src/lib/mv.c
bcc -ansi -c -o ./output/rm.o ./src/lib/rm.c
bcc -ansi -c -o ./output/ls.o ./src/lib/ls.c
bcc -ansi -c -o ./output/tictactoe.o ./src/lib/tictactoeOonga.c

ld86 -o ./bin/ln -d ./output/ln.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/cat -d ./output/cat.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/math.o ./output/fileIO.o
ld86 -o ./bin/cp -d ./output/cp.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/mkdir -d ./output/mkdir.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/mv -d ./output/mv.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/rm -d ./output/rm.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/ls -d ./output/ls.o ./output/lib_asm.o ./output/stringutil.o ./output/folderIO.o ./output/fileIO.o ./output/math.o
ld86 -o ./bin/tictactoe -d ./output/tictactoe.o ./output/lib_asm.o ./output/stringutil.o ./output/math.o ./output/fileIO.o ./output/folderIO.o