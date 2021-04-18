#ifndef folderIO_H
#define folderIO_H

int getPathIdx(int parentIdx, char *filename);

//Status code :
// -1 : Something went wrong in folder checking
// -2 : Folder exist but file doesn't
// -3 : Files sector is full
// pathidx : file found in files, 0 <= pathidx <= div(files sector size,16)
int getFilePathIdx(unsigned char parentIdx, char *filepath);
int SearchFilenames(char * files, char * filename, char parentIdx, int isFolder);

#endif