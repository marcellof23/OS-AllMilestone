#ifndef UTILITIES_H
#define UTILITIES_H

int mod(int x, int y);

int div(int x,int y);

int strcmp(char *string1,char *string2,int length);

void strslice(char *input,char *res,int start,int end);

int strlen(char *string);

void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int strsplit(char *input ,char param,char ptr[][64]);

#endif