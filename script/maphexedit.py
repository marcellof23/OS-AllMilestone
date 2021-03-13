with open("../output/map.img", 'rb') as f:
    data = f.read()

print(len(data))
filleddata = [0 for i in range(512)]
n = int(input("N bytes to fill: "))
for i in range(n):
    filleddata[i] = 0xFF

with open("../output/map.img", 'wb') as f:
    f.write(bytes(bytearray(filleddata)))