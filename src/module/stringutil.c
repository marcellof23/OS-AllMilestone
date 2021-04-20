#include "stringutil.h"

void itoa(int num, int basis, char * output) 
{ 
    int i = 0; 
    int isMinus = 0; 
    char str[100]; 
    int x,s,e,rem;
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        x = 0;
        while(str[x] != '\0')
   *output++ = str[x++];
  *output = '\0';
        return;
    } 
  
    if (num < 0 && basis == 10) 
    { 
        isMinus = 1; 
        num *= -1; 
    } 
    while (num != 0) 
    { 
        rem = mod(num, basis); 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/basis; 
    } 
  
    if (isMinus) 
        str[i++] = '-'; 
  
    str[i] = '\0';
    s = 0; 
    e = i -1; 
    while (s < e) 
    { 
        char tmp = *(str+s);
        *(str+s) = *(str+e);
        *(str+e) = tmp;
        s++; 
        e--; 
    }
    x = 0;
    while(str[x] != '\0')
  *output++ = str[x++];
 *output = '\0';
    return;
} 

int strcmp(char *string1,char *string2, int length) // Returns 1 if equal , 0 if not equal
{
  int i = 0;
  while(i<length)
  {
    if(string1[i] == 0)
    {
      return 1;
    }
    if(string1[i] != string2[i])
    {
      return 0;
    }
    i++;
  }
  return 1;
}

void strslice(char *input,char *res,int start,int end){
    int i;
    for(i=start;i<end;i++){
        res[i-start] = input[i];
    }
}

int isempty(char *buffer,int length){
    int i;
    char test[20];
    for(i=0;i<length;i++){
        if(buffer[i]!=0x0){
            return 0;
        }
    }
    return 1;
}

int strlen(char *string){
  int count = 0;
  while(1){
    if(string[count]=='\0'){
      break;
    }
    count++;
  }
  return count;
}

void clear(char *buffer, int length){
  int i;
	for (i = 0; i < length; i++)
	{
		buffer[i] = 0x00;
	}
}

int strsplit(char *input,char param,char ptr[][64])
{
    char command[8][64];
    int i=0;
    int j=0;
    int z=0;
    int commandcount=0;
    
    for(i=0;i<8;i++){
        for(j=0;j<64;j++){
            command[i][j] = '\0';
        }
    }

    i=0;
    j=0;
    
    while(1){
        if(input[i]=='\0'){
            commandcount++;
            break;
        } else if(input[i]==param){
            commandcount++;
            z=-1;
            j++;
        } else{
            command[j][z] = input[i];
        }
        i++;
        z++;
    }
    for(i=0;i<commandcount;i++){
        j=0;
        while(j<64){
            ptr[i][j] = command[i][j];
            j++;
        }
    }
    return commandcount;
}

void printString(char *string){
  int i= 0;
	while (*(string+i) != 0x00) {
		interrupt(0x10, 0x0e00 + *(string+i), 0x000F, 0, 0);
		i++;
	}
}

void readString(char *string){
  int input, i, j, idx, test, copyIdx;
  int high, low;
  i = 0;
  input = interrupt(0x16, 0x0000, 0, 0, 0);

  //Check payload, if there is payload then update string first
  if(string[0] == 0xFF && string[1] == 0xFF) {
    copyIdx = strlen(string) - 2;
    while(i < copyIdx) {
      string[i] = string[i+2];
      i++;
    }
  }

  high = input >> 8;
  low = input & 0x00FF; 
  
  while(low != 0x0d && low != 0x09 && high != 0x48 && high != 0x50) {
    if(low != 0x08){
      *(string+i) = low;
      i++;

      interrupt(0x10, 0x0e00 + low, 0, 0, 0);
    } else if(i > 0){
      interrupt(0x10, 0x0e00 + low, 0, 0, 0);
      interrupt(0x10, 0x0e00 + 0x0, 0, 0, 0);
      interrupt(0x10, 0x0e00 + low, 0, 0, 0);
      *(string+i-1) = 0x0;
      i--;
    }
    input = interrupt(0x16, 0x0000, 0, 0, 0);
    high = input >> 8;
    low = input & 0x00FF;
  }

  if(low == 0x00) {
    while(i > 0) {
      interrupt(0x10, 0x0e00 + 0x08, 0, 0, 0);
      interrupt(0x10, 0x0e00 + 0x0, 0, 0, 0);
      interrupt(0x10, 0x0e00 + 0x08, 0, 0, 0);
      *(string+i-1) = 0x0;
      i--;
    }
    string[0] = 0x00;
    string[1] = high;
    return;
  } else if(low == 0x09) {
    *(string+i) = 0x09;
    i++;
    *(string+i) = 0x0;
    return;
  }

  *(string+i) = 0x0;
}

void strcpy(char *buffer,char *arg){
  int i=0;
  while(arg[i]!=0x0){
    buffer[i] = arg[i];
    i++;
  }
}