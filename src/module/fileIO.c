#include "fileIO.h"
#include "folderIO.h"
#include "math.h"
#include "stringutil.h"

void readFile(char *buffer, char *path, int *result, char parentIndex) //      readFile(BX, CX, DX, AH);
{
  char files[1024];
  char sectorsFile[512];
  char fileName[14];
  int i=0;
  int j;
  int found=0;

  interrupt(0x21,2,files,0x101,0);
  interrupt(0x21,2,files+0x200,0x102,0);
  interrupt(0x21,2,sectorsFile,0x103,0);

  while(i<1024 && !found){
    strslice(files,fileName,i+2,i+16);
    if(files[i] == parentIndex && (unsigned char)files[i+1] != 0xFF && strcmp(fileName,path,14) && strlen(fileName)==strlen(path)){
      found = 1;
      for(j=0;j<16;j++){
        if(sectorsFile[files[i+1]*16+j] != 0x0){
          interrupt(0x21,2,buffer+j*512,sectorsFile[files[i+1]*16+j]);
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

  interrupt(0x21,2,map,0x100,0);
  interrupt(0x21,2,files,0x101,0);
  interrupt(0x21,2,files+0x200,0x102,0);
  interrupt(0x21,2,sectorsFile,0x103,0);

  if(files[parentIndex*16+1]!=0xFF && parentIndex != 0xFF){
    *sectors = -4;
    return;
  }

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

              interrupt(0x21,2,cache,j,0);
              for(z=0;z<512;z++){
                  cache[z] = buffer[k*512+z];
              }
              interrupt(0x21,3,cache,j,0);

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
      *sectors = -2;
    }
  }
  else{
    *sectors = -3;
  }

  interrupt(0x21,3,map,0x100,0);
  interrupt(0x21,3,files,0x101,0);
  interrupt(0x21,3,files+0x200,0x102,0);
  interrupt(0x21,3,sectorsFile,0x103,0);
}

void cleanSector(int sector){
  char buffer[512];
  int i=0;
  interrupt(0x21,2,buffer,sector,0);

  clear(buffer, 512);

  interrupt(0x21,3,buffer,sector,0);
}

void deleteFile(int idx){
  char map[512];
  char files[1024];
  char sectors[512];
  int i;
  int linked = 0;
  // interrupt(0x21,0,"It's a file\r\n",0,0);

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
      // interrupt(0x21,0,"File sector is safe to delete\r\n",0,0);
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
      // interrupt(0x21,0,"Some file is linked to the same file\r\n",0,0);
  }
  for(i=0;i<16;i++){
      files[idx*16+i] = 0x0;
  }

  interrupt(0x21,3,map,0x100,0);
  interrupt(0x21, 3, files, 0x101, 0);
  interrupt(0x21, 3, files+512, 0x102, 0);
  interrupt(0x21,3,sectors,0x103,0);
}