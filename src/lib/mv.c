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
        mv(command[3],command[4],parentIdx);
    } else{
        interrupt(0x21,0,"Usage : mv <source> <dest>\r\n");
    }

    deleteFile(getFilePathIdx(0xFF,"tmp/~temp"));
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

void mv(char *filename, char *target, int parentIdx) {
    char files[1024];
    char currFile[14];
    char pathList[64][64];
    char * outTest;
    int i = 0, j, newParentIdx = parentIdx, isFolder = 0, depth = 0, idx = 0;

    interrupt(0x21,2,files,0x101,0,0);
    interrupt(0x21,2,files+512,0x102,0,0);

    while(*(target+i) != 0x0) {
        if(*(target+i) == '/') {
            while(idx < 14) {
                pathList[depth][idx] = 0x0;
                idx++;
            }
            idx = 0;
            depth++;
        } else {
            pathList[depth][idx] = *(target+i);
            idx++;
        }
        i++;
    }
    
    while(idx < 14) {
        pathList[depth][idx] = 0x0;
        idx++;
    }

    for(j = 0; j <= depth; j++) {
        newParentIdx = getPathIdx(newParentIdx, pathList[j]);
        if(newParentIdx == -1) {
            break;
        }
    }

    if(newParentIdx != -1) {
        isFolder = 1;
    }

    if(isFolder) {
        // cek ada file dengan nama yang sama di folder yang dituju
        i = 0;
        while(i < 1024) {
            strslice(files, currFile, i+2, i+16);
            if(strcmp(filename, currFile, strlen(filename)) && strlen(filename) == strlen(currFile)) {
                if(files[i] == newParentIdx) {
                    interrupt(0x21, 0, "File with same name existed!!\r\n", 0, 0);
                    return;
                }
            }
            i += 16;
        }

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