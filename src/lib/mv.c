#include "../module/folderIO.h"
#include "../module/fileIO.h"
#include "../module/stringutil.h"
#include "../module/math.h"

void mv(char *filename, char *target, int parentIdx);

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

    if(argc==4){
        interrupt(0x21,0,"mv dipanggil\r\n",0,0);
        mv(command[3],command[4],parentIdx);
    } else{
        interrupt(0x21,0,"Usage : mv <source> <dest>\r\n");
    }

    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

void mv(char *filename, char *target, int parentIdx) {
    char files[1024];
    char currFile[14];
    char * outTest;
    int i, newParentIdx, isFolder = 0;

    interrupt(0x21,2,files,0x101,0,0);
    interrupt(0x21,2,files+512,0x102,0,0);

    // cari target
    i = 0;
    while(i < 1024) {
        strslice(files, currFile, i+2, i+16);
        if(strcmp(target, currFile, strlen(filename)) && strlen(target) == strlen(currFile)) {
            if((unsigned char)files[i+1] == 0xFF) {
                isFolder = 1;
                newParentIdx = div(i,16);
            }
        }
        i += 16;
    }

    if(isFolder) {
        // ngubah file directory
        i = 0;
        while(i < 1024) {
            strslice(files, currFile, i+2, i+16);
            if(strcmp(filename, currFile, strlen(filename)) && strlen(filename) == strlen(currFile)) {
                files[i] = (unsigned char) newParentIdx;
                interrupt(0x21, 3, files, 0x101, 0, 0);
                interrupt(0x21, 3, files+512, 0x102, 0, 0);
                break;
            }
            i += 16;
        }
    } else {
        // rename file
        i = 0;
        while(i < 1024) {
            strslice(files, currFile, i+2, i+16);
            if(strcmp(filename, currFile, strlen(filename)) && strlen(filename) == strlen(currFile)) {
                clear(files+i+2, 14);
                strslice(target, files+i+2, 0, strlen(target));

                interrupt(0x21, 3, files, 0x101, 0, 0);
                interrupt(0x21, 3, files+512, 0x102, 0, 0);
                break;
            }
            i += 16;
        }
    }
}