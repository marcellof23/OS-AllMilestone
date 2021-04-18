#ifndef FILEIO_H
#define FILEIO_H

void readSector(char *buffer,int sector);
void writeSector(char *buffer,int sector);
void readFile(char *buffer, char *path, int *sectors, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);

#endif