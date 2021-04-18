#include "fileIO.h"
#include "math.h"
#include "stringutil.h"

void readSector(char *buffer,int sector) {
  interrupt(0x13, 0x201, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}
void writeSector(char *buffer,int sector) {
  interrupt(0x13, 0x301, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) //      readFile(BX, CX, DX, AH);
{
  char files[1024];
  char sectorsFile[512];
  char fileName[14];
  int i=0;
  int j;
  int found=0;

  readSector(files,0x101);
  readSector(files+0x200,0x102);
  readSector(sectorsFile,0x103);

  while(i<1024 && !found){
    strslice(files,fileName,i+2,i+16);
    if(files[i] == parentIndex && (unsigned char)files[i+1] != 0xFF && strcmp(fileName,path,14) && strlen(fileName)==strlen(path)){
      found = 1;
      for(j=0;j<16;j++){
        if(sectorsFile[files[i+1]*16+j] != 0x0){
          readSector(buffer+j*512, sectorsFile[files[i+1]*16+j]);
        } else{
          clear(buffer+j*512, 512);
        }
      }
      break;
    }
    i+=16;
  }

  if(!found){
    *result = -1; 
  } else{
    *result = 1;
  }
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
  char map[512];
  char files[1024];
  char sectorsFile[512];
  char debugOutput[512];

  int i=0;

  readSector(map,0x100);
  readSector(files,0x101);
  readSector(files+0x200,0x102);
  readSector(sectorsFile,0x103);

  while(buffer[i]!=0x0){
    i++;
  }

  itoa(i,10,debugOutput);

  printString(debugOutput);

  writeSector(map,256);
  writeSector(files,257);
  writeSector(files+0x200,258);
  writeSector(sectorsFile,259);
}