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

void strcmp(char *string1,char *string2,int length)
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