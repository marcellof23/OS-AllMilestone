#include "module/math.h"
#include "module/stringutil.h"
#include "module/fileIO.h"
#include "module/folderIO.h"

int VIDEO_OFFSET=0x8000;
int VIDEO_SEGMENT=0xB000;
int VIDEO_SCREEN_SIZE = 4000;

void handleInterrupt21 (int AX, int BX, int CX, int DX);

void cls(int displaymode);

void executeProgram(char *filename, int segment, int *success, char parentIndex);
void readSector(char *buffer,int sector);
void writeSector(char *buffer,int sector);

int main () {
  char execStatus[16];

  makeInterrupt21();

  cls(3);

  interrupt(0x21,0x0006,"shell",0x3000,execStatus);
  while(1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
  char invalid[32];
  char AL, AH;

  clear(invalid,32);

  invalid[0] = 'I';invalid[1] = 'n';invalid[2] = 'v';invalid[3] = 'a';invalid[4] = 'l';invalid[5] = 'i';invalid[6] = 'd';

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
      writeFile(BX, CX, DX, AH); //void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
      break;
    case 0x6:
      executeProgram(BX, CX, DX, AH);
      break;
    default:
      interrupt(0x21,0,invalid,0,0);
  }
}

void cls(int displaymode){
  interrupt(0x10,displaymode,0,0,0);
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
    // Buat buffer
    int isSuccess;
    char fileBuffer[512 * 16];
    char fileNotFound[32];
    int i=0;
    char temp[10]; temp[0] = 't'; temp[1] = 'm';temp[2] ='p';temp[3] = '/';temp[4] = '~';temp[5] = 't';temp[6] = 'e';temp[7] = 'm';temp[8] = 'p';temp[9] = '\0';

    clear(fileNotFound,32);
    fileNotFound[0] = 'F' ; fileNotFound[1]= 'i'; fileNotFound[2] = 'l' ; fileNotFound[3]= 'e'; fileNotFound[4] = ' ' ; fileNotFound[5]= 'n'; fileNotFound[6]= 'o'; fileNotFound[7] = 't' ; fileNotFound[8]= ' '; fileNotFound[9] = 'f';fileNotFound[10] = 'o';fileNotFound[11] = 'u';fileNotFound[12] = 'n';fileNotFound[13] = 'd';fileNotFound[14] = '!'; fileNotFound[15] = '\r'; fileNotFound[16] = '\n';
    // Buka file dengan readFile
    readFile(&fileBuffer, filename, &isSuccess, parentIndex);
    // If success, salin dengan putInMemory
    if (isSuccess==1) {
        // launchProgram
        for (i = 0; i < 512*16; i++) {
            putInMemory(segment, i, fileBuffer[i]);
        }
        launchProgram(segment);
    } else {
        deleteFile(getFilePathIdx(0xFF,temp));
        interrupt(0x21, 0, fileNotFound, 0,0);
    }
}

void readSector(char *buffer,int sector) {
  interrupt(0x13, 0x201, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}
void writeSector(char *buffer,int sector) {
  interrupt(0x13, 0x301, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}