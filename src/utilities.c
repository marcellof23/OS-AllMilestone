#include <stdio.h>
int mod(int x,int y)
{
    while(x>=y)
    {
        x-=y;
    }
    return x;
}

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

int main() {
    printf("%d", mod(12,5));
    return 0;
}