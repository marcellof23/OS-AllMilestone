int VIDEO_OFFSET=0x8000;
int VIDEO_SEGMENT=0xB000;
int VIDEO_SCREEN_SIZE = 4000;


void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void printLogo();
void cls(int displaymode);

int main() {
  char *string = "OMAEWA";
  char *woi  = "WOIIIs";
  makeInterrupt21();
  handleInterrupt21(2,0x0013,0,0);
  printLogo();
  handleInterrupt21(0,string,0,0);
  handleInterrupt21(0,woi,0,0);

  // handleInterrupt21(2,"ADA",0,0);
  // handleInterrupt21(1,0,0,0);
  // handleInterrupt21(1,0,0,0);
  
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
  int i =0;
  while(*(string+i)!= '\0')
  {
    interrupt(0x10,0xE00 + *(string+i),0,0,0);
    ++i;
  }
  interrupt(0x10,0xE00 + '\n',0,0,0);
  interrupt(0x10,0xE00 + '\r',0,0,0);
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
  interrupt(0x10,displaymode,0,0);
}

void printLogo(){
  interrupt(10,0xc0d,0,0,0);
}
