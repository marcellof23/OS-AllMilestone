#include "shell.h"
#include "utilities.h"

void cwd(char pathIdx, char *dir) {
    int depth = 0;
    char pathOrder[64];
    char *absolutePath;
    char currDir = pathIdx;
    char file[512 * 2];
    int i, z,idx;
    int dirindex=0;
    interrupt(0x21, 0, "~", 0, 0);
    *(dir+dirindex) = '~'; dirindex++;
    if((unsigned char)currDir != 0xFF) {
        interrupt(0x21, 2, file, 0x101, 0);
        interrupt(0x21, 2, file + 512, 0x102, 0);

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
        for(z=0;z<idx;z++){
            *(dir+dirindex+z) = absolutePath[z];
        }
        dirindex += idx;
    }
    interrupt(0x21, 0, "$ ", 0, 0);
    *(dir+dirindex) = '$';
    *(dir+dirindex+1) = ' ';
    *(dir+dirindex+2) = 0x0;
}

int getPathIdx(char parentIdx, char *filename) { //Get Index file di files
    char file[512 * 2];
    char currFile[14];
    int i;
    interrupt(0x21, 2, file, 0x101, 0);
    interrupt(0x21, 2, file + 512, 0x102, 0);

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
    return -1;
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
        i++;
    }

    while(idx < 14) {
        *(pathList[depth] + idx) = 0x0;
    }

    parentIdx = *currParentIdx;
    for(j = 0; j <= depth; j++) {
        parentIdx = getPathIdx(parentIdx, pathList[j]);
        if(parentIdx == -1) {
            interrupt(0x21, 0, "No Such Directories\n\r", 0, 0);
            break;
        }
    }
    *currParentIdx = parentIdx;
}

void ln(char *filepath, char *filelink){
    
}

void ls(unsigned char parentIndex)
{
  char files[1024];
  char listFiles[64];
  char name[14];
  int i = 0,j = 0,total;
  char * filenames[64];
  interrupt(0x21, 2, files, 0x101, 0);
  interrupt(0x21, 2, files+512, 0x102, 0);
  while(i<64)
  {
    if(files[i*0x10] == parentIndex && files[i*0x10 + 2] != '\0')
    {
      *(listFiles+j) = i;
      j++;  
    }
    ++i;
  }
  total = j;
  for(i = 0; i<total;i++)
  {
    clear(name,14);
    for(j=0;j<14;j++)
    {
      name[j] = files[listFiles[i]  * 0x10 + 2 + j];
    }
    // filenames[i] = name;
    interrupt(0x21, 0, name, 0, 0);
    interrupt(0x21, 0, "\n", 0, 0);
    interrupt(0x21, 0, "\r", 0, 0);
  }
}

void cat(char * filenames, char dir)
{
    char buff[512 * 16];
    int pathIdx = getPathIdx(dir, filenames);
    if((unsigned char)pathIdx != 0xFF)
    {
        interrupt(0x21, 4, buff, 0x101, 0);
        interrupt(0x21, 0, filenames , 0, 0);
        return;
    }
    interrupt(0x21, 0, filenames , 0, 0);
    interrupt(0x21, 0, "bukan file", 0, 0);
}

void shell(){
    char command[128];
    unsigned char parentIdx = 0xFF;
    char dir[128];
    while(1){
        cwd(parentIdx,dir);
        interrupt(0x21,1,command,0,0);
        interrupt(0x21,0,dir,0,0);
        interrupt(0x21,0,command,0,0);
        interrupt(0x21,0,"\n\r",0,0);
        if(strcmp(command, "cd", strlen(command)) && strlen(command)==2){
            interrupt(0x21,0, "Cd dipanggil hahaha\n\r",0,0);
        } else if(strcmp(command, "ls", strlen(command)) && strlen(command)==2 ){
            interrupt(0x21,0, "Ls dipanggil hahaha\n\r",0,0);
            ls((unsigned char)parentIdx);
        } else if(strcmp(command,"cat",strlen(command)) && strlen(command)==3 ){
            interrupt(0x21,0, "Cat dipanggil hahaha\n\r",0,0);
        } else{
            interrupt(0x21,0, "Command tidak ketemu pala bapakkaooo\n\r");
        }
    }
}