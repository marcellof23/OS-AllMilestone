#include "fileIO.h"
#include "folderIO.h"
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

  char debugOutput[128];
  char crlf[3];

  char cache[512];

  int i,j,k,sectorsNeeded , sectorsAvailable,sectorIndex, filesIndex;

  int z=0;

  crlf[0] = '\r'; crlf[1] = '\n'; crlf[2] = '\0'; 

  filesIndex = -1;

  sectorsAvailable = 0;

  readSector(map,0x100);
  readSector(files,0x101);
  readSector(files+0x200,0x102);
  readSector(sectorsFile,0x103);

  i=0;
  while(buffer[i]!=0x0){
    i++;
  }

  sectorsNeeded = div(i,512) + 1;
  for(i=0;i<256;i++){
    if(map[i]!=0x0){
      sectorsAvailable++;
    }
  }

  if(sectorsAvailable>=sectorsNeeded){
    for(i=0;i<64;i++){
      if(isempty(files+i*16, 16)){
        filesIndex = i;
        break;
      }
    }
    if(filesIndex!=-1){
      for(i=0;i<32;i++){
        if(isempty(sectorsFile+i*16,16)){
          sectorIndex = i;
          j = 0;
          k = 0;

          while(j<256 && k<sectorsNeeded){
            if(map[j]==0x0){
              map[j] = 0xFF;
              sectorsFile[sectorIndex*16+k] = j;

              readSector(cache, j);
              for(z=0;z<512;z++){
                  cache[z] = buffer[k*512+z];
              }
              writeSector(cache, j);

              k++;
            }
            j++;
          }

          files[filesIndex*16] = parentIndex;
          files[filesIndex*16+1] = sectorIndex;

          j=2;
          while(j<16 && path[j-2]!=0x0){
            files[filesIndex*16+j] = path[j-2];
            j++;
          }
          break;
        }
      }
    } else{
      interrupt(0x21,0,"No slot for file\r\n",0,0);
    }
  }
  else{
    interrupt(0x21,0,"Not enough sectors\r\n",0,0);
  }

  writeSector(map,256);
  writeSector(files,257);
  writeSector(files+0x200,258);
  writeSector(sectorsFile,259);
}

void cleanSector(int sector){
  char buffer[512];
  int i=0;
  readSector(buffer, sector);

  clear(buffer, 512);

  writeSector(buffer, sector);
}

void deleteFile(int idx){
  char map[512];
  char files[1024];
  char sectors[512];
  int i;
  int linked = 0;
  interrupt(0x21,0,"It's a file\r\n",0,0);

  interrupt(0x21,2,map,0x100,0);
  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files+512, 0x102, 0);
  interrupt(0x21,2,sectors,0x103,0);
  i=0;
  while(i<64){
      if(i!=idx && files[i*16+1]==files[idx*16+1] && files[i*16+2]!=0x0){
          linked = 1;
          break;
      }
      i++;
  }
  if(!linked)
  {
      interrupt(0x21,0,"File sector is safe to delete\r\n",0,0);
      for(i=0;i<16;i++){
          if(sectors[files[idx*16+1]*16+i] != 0x0 && map[sectors[files[idx*16+1]*16+i]] != 0x0){
              cleanSector(sectors[files[idx*16+1]*16+i]);
              map[sectors[files[idx*16+1]*16+i]] = 0x0;
              sectors[files[idx*16+1]*16+i] = 0x0;
          }
      }
  } 
  else
  {
      interrupt(0x21,0,"Some file is linked to the same file\r\n",0,0);
  }
  for(i=0;i<16;i++){
      files[idx*16+i] = 0x0;
  }

  interrupt(0x21,3,map,0x100,0);
  interrupt(0x21, 3, files, 0x101, 0);
  interrupt(0x21, 3, files+512, 0x102, 0);
  interrupt(0x21,3,sectors,0x103,0);
}