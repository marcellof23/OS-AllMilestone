void printString(char *string);

void printString(char *string){
  int i=0;
  while(*(string+i)!='\0'){
    interrupt(0x10,0xe00 + *(string+i),0,0,0);
    i++;
  }
  // interrupt(0x10,0xe00 + '\n',0,0,0);
  // interrupt(0x10,0xe00 + '\r',0,0,0);
}

int main(){
    char x ='A';
    interrupt(0x10,0xe00,0,0,0);
    return 0;
}

