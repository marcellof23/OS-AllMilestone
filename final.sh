rm output/system.img

./compile.sh

dd if=/dev/zero of=./output/map.img bs=512 count=1
dd if=/dev/zero of=./output/files.img bs=512 count=2
dd if=/dev/zero of=./output/sectors.img bs=512 count=1

python3 script/maphexedit.py

dd if=./output/map.img of=./output/system.img bs=512 count=1 seek=256 conv=notrunc
dd if=./output/files.img of=./output/system.img bs=512 count=2 seek=257 conv=notrunc
dd if=./output/sectors.img of=./output/system.img bs=512 count=1 seek=259 conv=notrunc

python3 script/loadfile.py ./bin/hello_linked
