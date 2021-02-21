void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
  char *string = "OMAEWA\n";
  makeInterrupt21();
  handleInterrupt21(0,string,0,0);
  handleInterrupt21(2,"ADA",0,0);
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

  printString("\n\0");

  printString(testString);
}

void clear(char *buffer, int length){
  interrupt(0x06,00,00,00,00);
}
