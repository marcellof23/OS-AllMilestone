#include "module/math.h"
#include "module/stringutil.h"
#include "module/folderIO.h"

void cwd(int pathIdx,char *dir);

int cd(int currParentIdx, char *dirPath);

void ls(unsigned char parentIndex);

void mkdir( char *filenames,unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void messageArguments(char *argv,char parentIndex);

int main(){
    int i, j, commandCount, historyCount = -1, historyIdx = -1, count, idx;
    int tabPressed = 0, arrowPressed = 0;
    char input[128];
    char temp[128];
    char history[4][128];
    char command[8][64];
    int parentIdx = 0xFF;
    int targetDir;
    char dir[128];

    char execStatus[16];
        
    for(i = 0; i < 4; i++) {
        clear(history[i], 128);
    }

    while(1){
        if(!tabPressed && !arrowPressed) {
            cwd(parentIdx,dir);
        }

        interrupt(0x21,1,input,0,0);
        interrupt(0x21,0,"\r\n",0,0);
        commandCount = strsplit(input,' ',command);

        if(strcmp(command[0], "cd", strlen(command[0])) && strlen(command[0])==2){
            targetDir = cd(parentIdx,command[1]);
            if(targetDir != -1) {
                parentIdx = targetDir;
            } else {
                interrupt(0x21,0, "No such directory\r\n",0,0);
            }
        } else if(strcmp(command[0],"ls",strlen(command[0])) && strlen(command[0])==2){
            ls(parentIdx);
        } else{
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

int getPathIdx(int parentIdx, char *filename) { //Get Index file di files
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
            if(strcmp(filename, currFile, strlen(filename)) && (unsigned char)parentIdx == file[i]) {
                if((unsigned char)file[i+1] == 0xFF) {
                    return div(i,16);
                } else {
                    return -1;
                }
            }
            i += 16;
        }
        return -1;
    }
}

int getFilePathIdx(unsigned char parentIdx, char *filepath){
    char file[1024];
    char currFile[14];
    char pathchunks[32][64];
    int i,j;
    int totalchunks = strsplit(filepath,'/', pathchunks);
    int fileidxlooper = parentIdx;

    interrupt(0x21, 2, file, 0x101, 0);
    interrupt(0x21, 2, file + 512, 0x102, 0);
    for(i=0;i<totalchunks;i++){
        if(fileidxlooper == -1){
            return -1;
        }
        if(i==totalchunks-1){ //Finale
            for(j=0;j<1024;j+=16){
                if(file[j]==fileidxlooper){ // && (unsigned char)file[j+1]!=0xFF
                    strslice(file,currFile,j+2,j+16);
                    if(strcmp(currFile,pathchunks[i],strlen(currFile)) && strlen(currFile)==strlen(pathchunks[i])){
                        return div(j,16);
                    }
                }
            }
            return -2;
        } else{ //Folder
            fileidxlooper = getPathIdx(fileidxlooper, pathchunks[i]);
        }
    }
    return 999; // Shouldn't be possible to reach 999, only for debugging purposes
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
    char buffer[512 * 8];
    char trash = 0;
    clear(buffer,512 * 8);
    interrupt(0x21,0x0005,argv,"temp",&trash);
}