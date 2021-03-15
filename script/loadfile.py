# This script are meant to be run individually, separated from the Operating System

import os

with open("../output/system.img", 'rb') as f:
    system = f.read()
    system = bytearray(system)
    print("System length" ,len(system))
    map = system[256*512:257*512]
    print("Initial map:")
    print(map)
    files = system[257*512:259*512]
    sector = system[259*512:260*512]

# filepath = input("Input filepath: ")
filepath = "../misc/a.txt"
x = filepath.split("/")
filename = x[-1]
print(filename)

with open(filepath, 'rb') as f:
    fileloaded = f.read()
    fileloaded = bytearray(fileloaded)
    print("File length:",len(fileloaded))

sectorneeded = 0
filechunk = [[0x0 for i in range(512)] for j in range(16)]

for i in range(0,16):
    filechunk[i] = fileloaded[i*512:(i+1)*512]
    print("File chunks: ",filechunk[i])
    print(len(fileloaded[i*512:(i+1)*512]))
    if(filechunk[i]):
        sectorneeded+=1

print("Sector needed :" ,sectorneeded)
emptysector = 0
filesectorfound = False
filesectorindex = 0

for i in range(len(map)):
    if(map[i]==0x0):
        emptysector+=1

for i in range(64):
    if(files[i*16:(i+1)*16]==bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
        filesectorfound = True
        filesectorindex = i
        break

print("Empty sectors available:",emptysector)
if(emptysector-256>sectorneeded and filesectorfound):
    print("Executing load file...")
    sectors = [0 for i in range(16)]
    i = 0
    counter = 0
    while(i<512 and counter<sectorneeded):
        if(map[i]==0x0):
            map[i] = 0xFF #MARK sector used in map
            sectors[counter] = i #Save list of sectors in sectors
            counter+=1
        i+=1
    print("Final map:")
    system[256*512:257*512] = map
    print("System length" ,len(system))
    print(system[256*512:257*512])
    sectorsindex=0
    for i in range(32):
        if(sector[i*16:(i+1)*16]==bytearray(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')):
            sectorsindex = i
            for j in range(16):
                sector[i*16+j] = sectors[j]
            break
    
    system[259*512:260*512] = sector
    print("System length" ,len(system))
    files[filesectorindex*16] = 0xFF
    files[filesectorindex*16+1] = sectorsindex
    for i in range(len(filename)):
        files[filesectorindex*16+2+i] = ord(filename[i])
    system[257*512:259*512] = files
    print("System length" ,len(system))
    for i in range(sectorneeded):
        system[sectors[i]*512:(sectors[i]+1)*512] = bytes(filechunk[i]).ljust(512,b'\0')
        print("Chunk",i,"is",system[sectors[i]*512:(sectors[i]+1)*512])

    print("System length" ,len(system))
    with open("../output/system.img", 'wb') as f:
        f.write(bytes(system))

else:
    print("Not enough memory to load file")
