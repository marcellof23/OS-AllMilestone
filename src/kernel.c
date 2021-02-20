void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
  makeInterrupt21();
  handleInterrupt21(0,1,1,1);
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
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
  interrupt(10,0xff,0xff,0xff);
}

void readString(char *string){

}

void clear(char *buffer, int length){

}
