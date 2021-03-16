#include "utilities.h"

int div(int x,int y)
{
    int res;
    res = 0;
    while(x>=y)
    {
        x-=y;
        res++;
    }
    return res;
}

int mod(int x,int y) //Returns x mod y
{
    return x - y*div(x,y);
}

int strcmp(char *string1,char *string2,int length) // Returns 1 if equal , 0 if not equal
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

int strlen(char *string){
  int count = 0;
  while(1){
    if(string[count]==0x0){
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

void strsplit(char *input,char ptr[][64])
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
        } else if(input[i]==' '){
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
        while(j<64 && command[i][j]!='\0'){
            ptr[i][j] = command[i][j];
            j++;
        }
    }
}