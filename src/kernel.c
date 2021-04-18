#include "module/math.h"
#include "module/stringutil.h"
#include "module/fileIO.h"

int VIDEO_OFFSET=0x8000;
int VIDEO_SEGMENT=0xB000;
int VIDEO_SCREEN_SIZE = 4000;

void handleInterrupt21 (int AX, int BX, int CX, int DX);

void cls(int displaymode);

void executeProgram(char *filename, int segment, int *success, char parentIndex);


int main () {
  char execStatus[16];

  makeInterrupt21();

  cls(3);

  interrupt(0x21,0x0006,"shell",0x3000,execStatus);
  while(1);
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
      writeFile(BX, CX, DX, AH); //void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
      break;
    case 0x6:
      executeProgram(BX, CX, DX, AH);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void cls(int displaymode){
  interrupt(0x10,displaymode,0,0,0);
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
    // Buat buffer
    int isSuccess;
    char fileBuffer[512 * 16];
    int i=0;
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
        interrupt(0x21, 0, "File not found!", 0,0);
    }
}
