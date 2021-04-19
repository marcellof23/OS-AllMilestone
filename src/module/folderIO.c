#include "folderIO.h"
#include "fileIO.h"
#include "stringutil.h"
#include "math.h"

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

void getArguments(char *argv){
    char files[1024];
    char sectors[512];
    int i=0;
    int j=0;
    int parentIndex;

    char debugOutput[8];
    clear(debugOutput,8);

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);
    interrupt(0x21,2,sectors,0x103,0);

    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    parentIndex = i;

    i=0;
    while(i<64){
        if(files[i*16]==parentIndex && files[i*16+2]=='~' && files[i*16+3]=='t'){
            for(j=0;j<16;j++){
                interrupt(0x21,2,argv+j*512,sectors[files[i*16+1]*16+j],0);
            }
            break;
        }
        i++;
    }
}

void deleteFolder(int idx){
    int i;
    int empty = 1;
    char files[1024];
    interrupt(0x21,0,"It's a folder\r\n",0,0);

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);
    
    for(i=0;i<64;i++){
        if(files[i*16]==idx && !isempty(files+i*16,16)){
            empty = 0;
            break;
        }
    }

    if(empty){
        for(i=0;i<16;i++){
            files[idx*16+i] = 0x0;
        }
    } else{
        interrupt(0x21,0,"Folder is not empty, try using -r flag\r\n",0,0);
    }

    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files+512, 0x102, 0);
}