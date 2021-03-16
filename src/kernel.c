#include "utilities.h"
#include "shell.h"

extern imageFile;
int VIDEO_OFFSET=0x8000;
int VIDEO_SEGMENT=0xB000;
int VIDEO_SCREEN_SIZE = 4000;
char *image = &imageFile;

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void printStringNoNewline(char *string);
void readString(char *string);
void printLogo(int x, int y);
void printOSName();
void cls(int displaymode);
void readSector(char *buffer,int sector);
void writeSector(char *buffer,int sector);
void readFile(char *buffer, char *path, int *sectors, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);


int main () {
  makeInterrupt21();
  cls(0x13);
  printLogo(image[0],image[1]);
  handleInterrupt21(0x1,0,0,0);
  cls(3);
  printOSName();
  handleInterrupt21(0x1,0,0,0);
  cls(3);
  shell();
  while (1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
  char AL, AH;
  AL = (char) (AX);
  AH = (char) (AX >> 8);
  switch (AL) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02:
      readSector(BX, CX);
      break;
    case 0x03:
      writeSector(BX, CX);
      break;
    case 0x04:
      readFile(BX, CX, DX, AH);
      break;
    case 0x05:
      writeFile(BX, CX, DX, AH);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string){
  int i=0;
  while(*(string+i)!='\0'){
    interrupt(0x10,0xe00 + *(string+i),0,0,0);
    i++;
  }
  // interrupt(0x10,0xe00 + '\n',0,0,0);
  // interrupt(0x10,0xe00 + '\r',0,0,0);
}

void printStringNoNewline(char *string){
  int i=0;
  while(*(string+i)!='\0'){
    interrupt(0x10,0xe00 + *(string+i),0,0,0);
    i++;
  }
}

void readString(char *string){
  int input, i, j, idx, test;
  char filename[14];
  i = 0;
  input = interrupt(0x16, 0x0000, 0, 0, 0);
  while(input != 0x0d) {
    if(input != 0x8){
      interrupt(0x10, 0x0e00 + input, 0, 0, 0);
      *(string+i) = input;
      i++;
      input = interrupt(0x16, 0x0000, 0, 0, 0);
    } else {
      interrupt(0x10, 0x0e00 + input, 0, 0, 0);
      interrupt(0x10, 0x0e00 + 0x0,0,0);
      interrupt(0x10, 0x0e00 + input,0,0,0);
      *(string+i-1) = 0x0;
      i--;
      input = interrupt(0x16, 0x0000, 0, 0, 0);
    }
  }
  *(string+i) = 0x0;

  printString("\n");
  printString("\r");
}

void cls(int displaymode){
  interrupt(0x10,displaymode,0,0,0);
}

void printLogo(int x, int y){
  int i, j, idx;
  idx = 2;
  for(i = x; i > 0; i--) {
    for(j = y; j > 0; j--) {
      interrupt(0x10, 0x0c00 + (image[idx]), 0, i, j);
      idx++;
    }
  }
}

void printOSName(){
  printString(" /$$$$$$$ /$$  \r\n");
  printString("| $$__  $|__/ \r\n");
  printString("| $$  \\ $$/$$ /$$$$$$ \r\n");
  printString("| $$$$$$$| $$/$$__  $$   \r\n");
  printString("| $$____/| $| $$$$$$$$ \r\n");
  printString("| $$     | $| $$_____/ \r\n");
  printString("| $$     | $|  $$$$$$$ \r\n");
  printString("|/$$     |/$$\\_______/   \r\n");
  printString("| $$$    /$$$    \r\n");
  printString("| $$$$  /$$$$ /$$$$$$  /$$$$$$ /$$$$$$$\r\n");
  printString("| $$ $$/$$ $$/$$__  $$/$$__  $| $$__  $$\r\n");
  printString("| $$  $$$| $| $$  \\ $| $$  \\ $| $$  \\ $$\r\n");
  printString("| $$\\  $ | $| $$  | $| $$  | $| $$  | $$\r\n");
  printString("| $$ \\/  | $|  $$$$$$|  $$$$$$| $$  | $$\r\n");
  printString("|_/$$$$$$|_/$$$$$$__/ \\______/|__/  |__/\r\n");
  printString(" /$$__  $$/$$__  $$ \r\n");
  printString("| $$  \\ $| $$  \\__/ \r\n");
  printString("| $$  | $|  $$$$$$    \r\n");
  printString("| $$  | $$\\____  $$  \r\n");
  printString("| $$  | $$/$$  \\ $$  \r\n");
  printString("|  $$$$$$|  $$$$$$/  \r\n");
  printString(" \\______/ \\______/  \r\n");
}

void readSector(char *buffer,int sector) {
  interrupt(0x13, 0x201, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}
void writeSector(char *buffer,int sector) {
  interrupt(0x13, 0x301, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
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
    if(files[i] == parentIndex && (unsigned char)files[i+1] != 0xFF && strcmp(fileName,path,14)){
      found = 1;
      for(j=0;j<16;j++){
        if(sectorsFile[files[i+1]*16+j] != 0x0){
          readSector(buffer+j*512, sectorsFile[files[i+1]*16+j]);
        } else{
          clear(buffer+j*512, 512);
        }
      }
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
  int emptyIndex = 0;
  int totalSector;
  int i, j;
  int sektorkosong,tulis_sektor;

  readSector(map,0x100);
  readSector(files,0x101);
  readSector(files+0x200,0x102);
  readSector(sectorsFile,0x103);
  
  for(i=0;i<64;i++)
  {
    // filenya udah ada
    if((files[0x10*i] == parentIndex) && strcmp(path,files + (0x10*i) + 2,14))
    {
      *sectorsFile = -1;
      return;
    }
  }
  while(emptyIndex < 64)
  {
    if(files[emptyIndex * 0x10 + 2] == '\0')
      break;
    emptyIndex++;
  }
  // entrinya tidak cukup
  if(emptyIndex == 64)
  {
    *sectorsFile = -2;
    return;
  }
  // cek sektor penuh/ngga
  for(i = 0; i < 0x100 ;i++)
  {
    if(buffer[i] == 0)
    {
      totalSector++;
    }
  }
  if(totalSector<*sectors)
  {
    *sectorsFile = -3;
    return;
  }
  
  //cek sektor penuh 
  for(j = 0; j < 0x20 ;j++)
  {
    if(sectorsFile[j * 0x10] == '\0')
    {
      break;
    }
  }
  if(j == 0x20)
  {
    *sectorsFile = -3;
     return;
  }
  // cek indeks entri = 0xFF 
  if((unsigned char)files[parentIndex * 0x10 + 1] != 0xFF)
  {
    // parentIndex bukan root
    if((unsigned char)parentIndex != 0xFF)
    {
      *sectorsFile = -4;
      return;
    }
  }
  
  clear(files + (emptyIndex*0x10), 16);
  files[emptyIndex * 0x10] = parentIndex;
  files[emptyIndex * 0x10 + 1] = j;
  for(i=0;i<14;i++)
  {
    if(path[i] != 0x00)
    {
      files[i*0x10 + 2 + i] = path[i];
    }
    else
    {
      break;
    }
  }
  i = 0;
  while(i < *sectors)
  {
    for(sektorkosong = 0;sektorkosong < 0x100;sektorkosong++)
    {
      if(buffer[sektorkosong] == 0x00)
      {
        break;
      }
    }
    tulis_sektor = sektorkosong;
    map[sektorkosong] = 0xFF;
    sectorsFile[j * 0x10 + sektorkosong] = tulis_sektor;
    writeSector(buffer + (i * 512), tulis_sektor); 
    i++;
  }
  writeSector(map,256);
  writeSector(files,257);
  writeSector(files+0x200,258);
  writeSector(sectorsFile,259);
}

