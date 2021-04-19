#include "../module/folderIO.h"
#include "../module/fileIO.h"
#include "../module/stringutil.h"
#include "../module/math.h"

void rm(char *filename,unsigned char parentIndex);

void rmRecursive(char *filename,unsigned char parentIndex);

int main(){
    char buffer[512*16];
    char execStatus[16];
    char command[16][64];
    char argc,parentIdx;
    int commandCount,i;

    char debugOutput[128];

    clear(buffer,512*16);

    getArguments(buffer);

    commandCount = strsplit(buffer,' ',command);

    argc = command[0][0];
    parentIdx = command[1][0];

    if(argc==3){
        interrupt(0x21,0,"rm dipanggil\r\n",0,0);
        rm(command[3],parentIdx);
    } else if(argc==4){
        interrupt(0x21,0,"rm recursive dipanggil\r\n",0,0);
        interrupt(0x21,0, "parentIdx shell\r\n");

        itoa(parentIdx,10,debugOutput);
        interrupt(0x21,0,debugOutput,0,0);
        interrupt(0x21,0,"\r\n",0,0);
        rmRecursive(command[4],parentIdx);
    }
    else{
        interrupt(0x21,0,"Usage : rm <filepath>\r\n");
    }

    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

void rm(char *filename,unsigned char parentIndex){
    char map[512];
    char files[1024];
    char sectors[512];
    char debugOutput[64];
    int i=0;
    int idx;
    int linked=0;
    int empty = 1;

    interrupt(0x21,2,map,0x100,0);
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);
    interrupt(0x21,2,sectors,0x103,0);


    idx = getFilePathIdx(parentIndex, filename);

    itoa(idx,10,debugOutput);
    interrupt(0x21,0,debugOutput,0,0);
    interrupt(0x21,0,"\r\n",0,0);

    if(files[idx*16+1]==0xFF)
    {
        interrupt(0x21,0,"It's a folder\r\n",0,0);
        
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

    } 
    else{
        interrupt(0x21,0,"It's a file\r\n",0,0);
        i=0;
        while(i<64){
            if(i!=idx && files[i*16+1]==files[idx*16+1] && files[i*16+2]!=0x0){
                linked = 1;
                break;
            }
            i++;
        }
        if(!linked)
        {
            interrupt(0x21,0,"File sector is safe to delete\r\n",0,0);
            for(i=0;i<16;i++){
                if(sectors[files[idx*16+1]*16+i] != 0x0 && map[sectors[files[idx*16+1]*16+i]] != 0x0){
                    cleanSector(sectors[files[idx*16+1]*16+i]);
                    map[sectors[files[idx*16+1]*16+i]] = 0x0;
                    sectors[files[idx*16+1]*16+i] = 0x0;
                }
            }
        } 
        else
        {
            interrupt(0x21,0,"Some file is linked to the same file\r\n",0,0);
        }
        for(i=0;i<16;i++){
            files[idx*16+i] = 0x0;
        }
    }

    interrupt(0x21,3,map,0x100,0);
    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files+512, 0x102, 0);
    interrupt(0x21,3,sectors,0x103,0);
}

void rmRecursive(char *filename,unsigned char parentIndex){
    char files[1024];
    int i=0;
    int idx;
    char childFilename[14];
    char debugOutput[512];

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);

    interrupt(0x21,0,"HAHAHA\r\n",0,0);
    itoa(idx,10,debugOutput);
    interrupt(0x21,0,debugOutput,0,0);
    interrupt(0x21,0,"\r\n",0,0);

    idx = getFilePathIdx(parentIndex, filename);

    for(i=0;i<64;i++){
        if(files[i*16]==idx && !isempty(files+i*16, 16)){
            strslice(files+i*16,childFilename,2,16);
            interrupt(0x21,0,childFilename,0,0);
            interrupt(0x21,0,"\r\n",0,0);
            rmRecursive(childFilename, idx);
            clear(childFilename,14);
        }
    }

    rm(filename,parentIndex);
}