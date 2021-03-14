#ifndef SHELL_H
#define SHELL_H

void cwd(char pathIdx,char *dir);

void cd(char *currParentIdx, char *dirPath);

int getPathIdx(char parentIdx, char *filename);

void ln(char *filepath, char *filelink);

void ls(char parentIndex);

void cat(char * filenames, char dir);

void shell();

#endif