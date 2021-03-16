#ifndef SHELL_H
#define SHELL_H

void cwd(int pathIdx,char *dir);

int cd(int currParentIdx, char *dirPath);

int getPathIdx(int parentIdx, char *filename);

//Status code :
// -1 : Something went wrong in folder checking
// -2 : Folder exist but file doesn't
// -3 : Files sector is full
// pathidx : file found in files, 0 <= pathidx <= div(files sector size,16)
int getFilePathIdx(unsigned char parentIdx, char *filepath);

int ln(char *filepath, char *filelink,int soft,unsigned char parentIndex);

void ls(unsigned char parentIndex);

void cat(char * filenames,unsigned char dir);

void mkdir( char *filenames,unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void shell();

#endif