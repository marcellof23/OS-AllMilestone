#ifndef STRINGUTIL_H
#define STRINGUTIL_H

int strcmp(char *string1,char *string2,int length);

void strslice(char *input,char *res,int start,int end);

int strlen(char *string);

void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int strsplit(char *input ,char param,char ptr[][64]);

void itoa(int num, int basis, char * output);

int isempty(char *buffer,int length);

void printString(char *string);
void readString(char *string);

#endif