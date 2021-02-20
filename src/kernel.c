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
  int i=0;
  while(*(string+i)!='\0'){
    interrupt(0x10,0xe00 + *(string+i),0,0,0);
    i++;
  }
}

void readString(char *string){

}

void clear(char *buffer, int length){
  interrupt(0x06,00,00,00,00);
}
