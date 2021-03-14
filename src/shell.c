#include "shell.h"
#include "utilities.h"

void cwd(char pathIdx) {
    int depth = 0;
    char pathOrder[64];
    char *absolutePath;
    char currDir = pathIdx;
    char file[512 * 2];
    int i, idx;
    if((unsigned char)currDir != 0xFF) {
        interrupt(0x21, 3, file[0], 0x101, 0);
        interrupt(0x21, 3, file[512], 0x102, 0);

        interrupt(0x21, 0, "~/", 0, 0);

        // backtracking to root
        while((unsigned char)currDir != 0xFF) {
            pathOrder[depth] = currDir;
            depth++; 
            currDir = file[currDir << 4];
        }

        idx = 0;
        depth--;
        while(depth >= 0) {
            i = 0;
            while(file[(pathOrder[depth] << 4) + 2 + i] != 0) {
                *(absolutePath+idx) = file[(pathOrder[depth] << 4) + 2 + i];
                idx++;
                i++;
            }
            if(depth > 0) {
                *(absolutePath+idx) = '/';
                idx++;
            }
            depth--;
        }
        *(absolutePath+idx) = 0x00;
        interrupt(0x21, 0, absolutePath, 0, 0);
    }
    interrupt(0x21, 0, "$ ", 0, 0);
}

int getPathIdx(char parentIdx, char *filename) { //Get Index file di files
    char file[512 * 2];
    char currFile[14];
    int i;
    interrupt(0x21, 3, file[0], 0x101, 0);
    interrupt(0x21, 3, file[512], 0x102, 0);

    if(*(filename) == '~') {
        return 0xFF;
    } else if(*(filename) == '.' && *(filename+1) == '.') {
        if((unsigned char)parentIdx == 0xFF) {
            return -1;
        } else {
            return file[parentIdx << 4];
        }
    } else if(*(filename) == '.') {
        return parentIdx;
    } else {
        i = 0;
        while(i < 1024) {
            strslice(file, currFile, i+2, i+16);
            if(strcmp(filename, currFile, 14) && parentIdx == file[i]) {
                if((unsigned char)file[i+2] == 0xFF) {
                    return file[i];
                } else {
                    return -1;
                }
            }
            i += 16;
        }
    }
}

void cd(char *currParentIdx, char *dirPath) {
    char *pathList[64];
    int parentIdx;
    int i, j, idx, depth;
    i = 0;
    idx = 0;
    depth = 0;
    while(*(dirPath+i) != 0x0) {
        if(*(dirPath+i) == '/') {
            while(idx < 14) {
                *(pathList[depth] + idx) = 0x0;
            }
            idx = 0;
            depth++;
        } else {
            *(pathList[depth] + idx) = *(dirPath+i);
            idx++;
        }
    }

    parentIdx = *currParentIdx;
    for(j = 0; j <= depth; j++) {
        parentIdx = getPathIdx(parentIdx, pathList[j]);
        if(parentIdx == -1) {
            interrupt(0x21, 0, "No Such Directories", 0, 0);
            break;
        }
    }
    *currParentIdx = parentIdx;
}

void ln(char *filepath, char *filelink){
    
}

void ls(char parentIndex)
{
  char files[1024];
  char *listFiles;
  readSector(files,257);
  readSector(files+0x200,258);
  int i = 0,j = 0,total;
  while(i<64)
  {
    if(files[i*0x10] == parentIndex && files[i*0x10 + 2] != '\0')
    {
      j++;  
    }
    ++i;
  }
  total = j;
  char * str[64];
  for(i = 0; i<total;i++)
  {
    
    for(j=0;j<14;j++)
    {
      str[i][j] = files[i * 0x10 + 2 + j];
      if(files[i * 0x10 + 2 + j] == '\0')
      {
        break;
      }
    }
    printString(str[i]);
    i++;
  }
}