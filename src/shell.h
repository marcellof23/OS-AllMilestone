#ifndef SHELL_H
#define SHELL_H

void cwd(char pathIdx,char *dir);

void cd(unsigned char *currParentIdx, char *dirPath);

int getPathIdx(unsigned char parentIdx, char *filename);

void ln(char *filepath, char *filelink);

void ls(unsigned char parentIndex);

void cat(char * filenames, char dir);

void mkdir( char *filenames,unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void shell();

#endif