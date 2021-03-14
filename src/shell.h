#ifndef SHELL_H
#define SHELL_H

void cwd(char pathIdx);

void cd(char *command);

int getPathIdx(char pathIdx);

void ln(char *filepath, char *filelink);

#endif