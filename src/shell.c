#include "module/math.h"
#include "module/stringutil.h"
#include "module/folderIO.h"

void cwd(int pathIdx,char *dir);

void cat(char * filenames,unsigned char dir);

int cd(int currParentIdx, char *dirPath);

void ls(unsigned char parentIndex);

void mkdir( char *filenames,unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void messageArguments(char *argv,char parentIndex);

void shellState(char currParentIdx);

void getShellState(char *argv);

int main(){
    int i, j, commandCount, historyCount = -1, historyIdx = -1, count, idx;
    int tabPressed = 0, arrowPressed = 0;
    char input[512];
    char temp[128];
    char history[4][128];
    char command[8][64];
    int parentIdx = 0xFF;
    int targetDir;
    char dir[128];
    char currShellState[8192];
    char debugOutput[512];

    char execStatus[16];

    clear(input,512);
    clear(currShellState, 8192);
        
    for(i = 0; i < 4; i++) {
        clear(history[i], 128);
    }

    getShellState(currShellState);

    parentIdx = currShellState[0];

    while(1){
        if(!tabPressed && !arrowPressed) {
            cwd(parentIdx,dir);
        }

        clear(input,512);

        interrupt(0x21,1,input,0,0);
        interrupt(0x21,0,"\r\n",0,0);
        commandCount = strsplit(input,' ',command);

        if(strcmp(command[0], "cd", strlen(command[0])) && strlen(command[0])==2){
            targetDir = cd(parentIdx,command[1]);
            if(targetDir != -1) {
                parentIdx = targetDir;
                shellState((unsigned char) targetDir);
            } else {
                interrupt(0x21,0, "No such directory\r\n",0,0);
            }
        } else if(strcmp(command[0],"ls",strlen(command[0])) && strlen(command[0])==2){
            ls(parentIdx);
        } else if(strcmp(command[0],"cat",strlen(command[0])) && strlen(command[0])==3){
            cat(command[1],parentIdx);
        } 
        else{
            messageArguments(input,parentIdx);
            interrupt(0x21,0x0006,command[0],0x3000,execStatus);
        }
    }
    return 0;
}

void cwd(int pathIdx, char *dir) {
    int depth = 0;
    char pathOrder[64];
    char absolutePath[512];
    int currDir = pathIdx;
    char file[512 * 2];
    int i, z, idx = 0;
    int dirindex=0;
    interrupt(0x21, 0, "~", 0, 0);
    *(dir+dirindex) = '~'; dirindex++;
    if((unsigned char)currDir != 0xFF) {
        interrupt(0x21, 2, file, 0x101, 0);
        interrupt(0x21, 2, file + 512, 0x102, 0);

        // backtracking to root
        while(currDir != 0xFF) {
            pathOrder[depth] = currDir;
            depth++;
            currDir = file[currDir << 4];
        }
        *(absolutePath+idx) = '/';
        idx++;

        while(depth != 0) {
            depth--;
            i = 0;
            while(file[(pathOrder[depth] << 4) + 2 + i] != 0) {
                *(absolutePath+idx) = file[(pathOrder[depth] << 4) + 2 + i];
                idx++;
                i++;
            }
            if(depth != 0) {
                *(absolutePath+idx) = '/';
                idx++;
            }
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

int cd(int currParentIdx, char *dirPath) {
    char pathList[64][64];
    int parentIdx;
    int i, j, idx, depth;
    i = 0;
    idx = 0;
    depth = 0;
    
    while(*(dirPath+i) != 0x0) {
        if(*(dirPath+i) == '/') {
            while(idx < 14) {
                pathList[depth][idx] = 0x0;
                idx++;
            }
            idx = 0;
            depth++;
        } else {
            pathList[depth][idx] = *(dirPath+i);
            idx++;
        }
        i++;
    }
    
    while(idx < 14) {
        pathList[depth][idx] = 0x0;
        idx++;
    }

    parentIdx = currParentIdx;
    for(j = 0; j <= depth; j++) {
        parentIdx = getPathIdx(parentIdx, pathList[j]);
        if(parentIdx == -1) {
            break;
        }
    }
    return parentIdx;
}

void ls(unsigned char parentIndex)
{
  char files[1024];
  char listFiles[64];
  char name[14];
  int i = 0,j = 0,total;
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

    interrupt(0x21, 0, name, 0, 0);
    interrupt(0x21, 0, "\n", 0, 0);
    interrupt(0x21, 0, "\r", 0, 0);
  }
}

void autoComplete(char *filename, char parentIdx) {
    char files[512 * 2];
    char *temp;
    char *currFilename;
    int i, idxFilename, idxTemp;
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files + 512, 0x102, 0);

    i = 0;
    while(i < 1024) {
        strslice(files, currFilename, i+2, i+16);
        if(files[i] == parentIdx && strcmp(filename, currFilename, strlen(filename))) {
            strslice(files, temp, i+2+strlen(filename), i+16);
            interrupt(0x21, 0, temp, 0, 0);

            //update filename
            idxFilename = strlen(filename);
            idxTemp = 0;
            while(*(temp+idxTemp) != 0x0) {
                *(filename+idxFilename) = *(temp+idxTemp);
                idxFilename++;
                idxTemp++;
            }
            *(filename+idxFilename) = 0x0;
            break;
        }
        i += 16;
    }
}

void messageArguments(char *argv,char parentIndex){
    char files[1024];
    char debugOutput[16];
    char trash = 0;
    int i=0;
    int argc=2;

    char arg[512];
    clear(arg,512);

    for(i=0;i<512;i++){
        if(argv[i]==' '){
            argc++;
        }
    }

    arg[0] = argc;
    arg[1] = ' ';

    arg[2] = parentIndex;
    arg[3] = ' ';

    for(i=4;i<512;i++){
        arg[i] = argv[i-4];
    }

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x5,arg,"~temp",&trash);
}

void shellState(char currParentIndex){
    char files[1024];
    char debugOutput[16];
    char trash = 0;
    int i=0;

    char arg[512];
    clear(arg,512);

    arg[0] = currParentIndex;
    itoa(currParentIndex, 16, debugOutput);
    interrupt(0x21, 0, debugOutput, 0, 0);

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x5,arg,"~shell",&trash);
}

void getShellState(char *argv) {
    char files[1024];
    char debugOutput[16];
    int flag = 0;
    int i=0;

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x4,argv,"~shell",&flag);

    if(flag == -1) {
        argv[0] = 0xFF;
    }
}

void cat(char * filenames, unsigned char parentIdx)
{
    char files[1024];
    char buff[512 * 16];
    int idx = strlen(filenames);
    int pathIdx;
    char output[100];
    char filematrix[64][64];
    char countsplit;
    char linkedname[17];
    // char * currfile;
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);
    while(idx < 14) {
        *(filenames + idx) = 0x0;
        idx++;
    }
    clear(output,100);
    pathIdx = getFilePathIdx(parentIdx, filenames);
    countsplit = strsplit(filenames, '/', filematrix);

    if(files[0x10*pathIdx+1]<0x20){
        if(pathIdx>=0)
        {
            interrupt(0x21, 4 + 0x100*files[0x10*pathIdx], buff, filematrix[countsplit-1], 0);
            interrupt(0x21, 0, buff , 0, 0);
            interrupt(0x21, 0, "\r\n" , 0, 0);
            return;
        }
    } else{
        if(pathIdx>=0)
        {
            strslice(files+0x10*(files[0x10*pathIdx+1]-0x20),linkedname,2,16);
            interrupt(0x21, 4 + 0x100*(files[0x10*(files[0x10*pathIdx+1]-0x20)]), buff, linkedname, 0);
            interrupt(0x21, 0, buff , 0, 0);
            interrupt(0x21,0,"\r\n", 0,0);
            return;
        }       
    }
    interrupt(0x21, 0, "file tidak ditemukan\r\n", 0, 0);
}