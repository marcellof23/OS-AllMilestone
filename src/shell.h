#ifndef SHELL_H
#define SHELL_H

void cwd(char pathIdx,char *dir);

void cd(unsigned char *currParentIdx, char *dirPath);

int getPathIdx(unsigned char parentIdx, char *filename);

void ln(char *filepath, char *filelink);

void ls(unsigned char parentIndex);

void cat(char * filenames, char dir);

<<<<<<< HEAD
void mkdir( char *filenames,unsigned char parentIndex);
=======
void autoComplete(char *filename, char parentIdx);
>>>>>>> a644fb39bbac5b209c39fb7d0ac09b265dec9622

void shell();

#endif