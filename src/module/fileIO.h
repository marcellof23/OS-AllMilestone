#ifndef FILEIO_H
#define FILEIO_H

void readFile(char *buffer, char *path, int *sectors, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);

void cleanSector(int sector);

void deleteFile(int idx);

#endif