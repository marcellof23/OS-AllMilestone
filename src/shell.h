#ifndef SHELL_H
#define SHELL_H

void cwd(char pathIdx);

void cd(char *currParentIdx, char *dirPath);

int getPathIdx(char parentIdx, char *filename);

void ln(char *filepath, char *filelink);

#endif