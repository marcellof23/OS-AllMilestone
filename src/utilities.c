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

void fillzero(char *buffer,int length){
    int i;
    for(i=0;i<length;i++){
        *(buffer+i) = 0x0;
    }
}