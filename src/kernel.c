#include "utilities.h"

extern imageFile;
int VIDEO_OFFSET=0x8000;
int VIDEO_SEGMENT=0xB000;
int VIDEO_SCREEN_SIZE = 4000;
char *image = &imageFile;

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
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
  while (1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      cls(BX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string){
  // Menggunakan int 10h AH = 02h dan 09h
  
  // int i = 0;
  // while(*(string+i) != '\0') {
  //   interrupt(0x10, 0x0200, 0, 0, i);
  //   interrupt(0x10, 0x0900 + *(string+i), 0x02, 1, 0);
  //   i++;
  // }
  
  // Menggunakan int 10h AH = 0eh

  int i=0;
  while(*(string+i)!='\0'){
    interrupt(0x10,0xe00 + *(string+i),0,0,0);
    i++;
  }
  interrupt(0x10,0xe00 + '\n',0,0,0);
  interrupt(0x10,0xe00 + '\r',0,0,0);
}

void readString(char *string){
  int input, i, test;
  char *testString;
  i = 0;
  input = interrupt(0x16, 0x0000, 0, 0, 0);
  while(input != 0x0d) {
    interrupt(0x10, 0x0e00 + input, 0, 0, 0);
    *(testString+i) = input;
    i++;
    input = interrupt(0x16, 0x0000, 0, 0, 0);
  }
  *(testString+i) = 0x0;

  printString("\r");

  printString(testString);
}

void clear(char *buffer, int length){
  int i;
	for (i = 0; i < length; i++)
	{
		buffer[i] = 0x00;
	}
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
  printString(" /$$$$$$$ /$$  ");
  printString("| $$__  $|__/ ");
  printString("| $$  \\ $$/$$ /$$$$$$ ");
  printString("| $$$$$$$| $$/$$__  $$   ");
  printString("| $$____/| $| $$$$$$$$ ");
  printString("| $$     | $| $$_____/ ");
  printString("| $$     | $|  $$$$$$$ ");
  printString("|/$$     |/$$\\_______/   ");
  printString("| $$$    /$$$    ");
  printString("| $$$$  /$$$$ /$$$$$$  /$$$$$$ /$$$$$$$");
  printString("| $$ $$/$$ $$/$$__  $$/$$__  $| $$__  $$");
  printString("| $$  $$$| $| $$  \\ $| $$  \\ $| $$  \\ $$");
  printString("| $$\\  $ | $| $$  | $| $$  | $| $$  | $$");
  printString("| $$ \\/  | $|  $$$$$$|  $$$$$$| $$  | $$");
  printString("|_/$$$$$$|_/$$$$$$__/ \\______/|__/  |__/");
  printString(" /$$__  $$/$$__  $$ ");
  printString("| $$  \\ $| $$  \\__/ ");
  printString("| $$  | $|  $$$$$$    ");
  printString("| $$  | $$\\____  $$  ");
  printString("| $$  | $$/$$  \\ $$  ");
  printString("|  $$$$$$|  $$$$$$/  ");
  printString(" \\______/ \\______/  ");
}

void readSector(char *buffer,int sector) {
  interrupt(0x13, 0x201, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}
void writeSector(char *buffer,int sector) {
  interrupt(0x13, 0x301, buffer, div(sector,36)*0x100 + mod(sector,18) + 1, mod(div(sector,18),2)*0x100);
}
void readFile(char *buffer, char *path, int *sectors, char parentIndex)
{

}
void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
  char map[512];
  char files[512];
  char sectorsFile[1024];

  readSector(map,256);
  readSector(files,0x101);
  readSector(files+0x200,0x102);
  readSector(sectorsFile,0x103);
}