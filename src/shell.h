#ifndef SHELL_H
#define SHELL_H

void cwd(int pathIdx,char *dir);

int cd(int currParentIdx, char *dirPath);

int getPathIdx(int parentIdx, char *filename);

int ln(char *filepath, char *filelink,int soft,unsigned char parentIndex);

void ls(unsigned char parentIndex);

void cat(char * filenames, char dir);

void mkdir( char *filenames,unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void shell();

#endif