#include "math.h"

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