dd if=./output/map.img of=./output/system.img bs=512 count=1 seek=256 conv=notrunc
dd if=./output/files.img of=./output/system.img bs=512 count=2 seek=257 conv=notrunc
dd if=./output/sectors.img of=./output/system.img bs=512 count=1 seek=259 conv=notrunc