#include "../module/folderIO.h"
#include "../module/stringutil.h"
#include "../module/math.h"

void cat(char * filenames,unsigned char dir);

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

    if(argc!=3){
        interrupt(0x21,0,"Usage : cat <filepath>\r\n");
    } else{
        cat(command[3], parentIdx);
    }

    // interrupt(0x21,0,buffer,0,0);
    // interrupt(0x21,0,"\r\n",0,0);

    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

void cat(char * filenames, unsigned char parentIdx)
{
    char files[1024];
    char buff[512 * 16];
    int idx = strlen(filenames);
    int pathIdx,counterBuff = 0;
    char output[100];
    char filematrix[64][64];
    char countsplit;
    char buffers[2];
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
    clear(buff,512*16);
    if(files[0x10*pathIdx+1]<0x20){
        if(pathIdx>=0)
        {
            interrupt(0x21, 4 + 0x100*files[0x10*pathIdx], buff, filematrix[countsplit-1], 0);
            while(counterBuff < 512*16 && buff[counterBuff] != '\0')
            {
                buffers[0] = buff[counterBuff];
                buffers[1] = 0;
                if(buff[counterBuff] != '\n')
                {    
                    interrupt(0x21, 0, buffers , 0, 0);
                }
                else 
                {
                    interrupt(0x21, 0, "\r" , 0, 0);
                    interrupt(0x21, 0, buffers , 0, 0);      
                }
                counterBuff++;
            }
            
            // interrupt(0x21, 0, buff , 0, 0);
            interrupt(0x21, 0, "\r\n" , 0, 0);
            return;
        }
    } else{
        if(pathIdx>=0)
        {
            strslice(files+0x10*(files[0x10*pathIdx+1]-0x20),linkedname,2,16);
            cat(linkedname,(files+0x10*(files[0x10*pathIdx+1]-0x20))[0]);
            return;
        }       
    }
    interrupt(0x21, 0, "file tidak ditemukan\r\n", 0, 0);
}