#include "../module/folderIO.h"
#include "../module/fileIO.h"
#include "../module/stringutil.h"
#include "../module/math.h"

void cpFiles(char * filenames, char parentIdx, char * src, char * dest);
void cponly(char * filenames, char parentIdx1,char parentIdx2, char * src, char * dest);
void cp(char * buf, char parentIdx1 , char parentIdx2, char * src, char * dest);

int main(){
    char buffer[512*16];
    char execStatus[16];
    char command[16][64];
    char argc,parentIdx;
    int commandCount,i,idxD,idxS;

    char debugOutput[128];

    clear(buffer,512*16);

    getArguments(buffer);

    commandCount = strsplit(buffer,' ',command);

    argc = command[0][0];
    parentIdx = command[1][0];

    if(argc==4){
        idxS = getFilePathIdx(parentIdx, command[3]);
        idxD = getFilePathIdx(parentIdx, command[4]);
        if(idxS == idxD && strcmp(command[3], command[4],14))
        {
            interrupt(0x21,0,"You copy two file with the same name\r\n",0,0);
        }
        else
        {
            cpFiles(buffer,parentIdx, command[3], command[4]);
        } 
         
    } else if(argc==5){
        cp(buffer, parentIdx, parentIdx, command[4], command[5]);
    }
    else{
        interrupt(0x21,0,"Usage : cp <src> <dest>\r\n");
        interrupt(0x21,0,"or Use : cp -r <src> <dest> for directory\r\n");
    }

    deleteFile(getFilePathIdx(0xFF,"tmp/~temp"));
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

void cpFiles(char * filenames, char parentIdx, char * src, char * dest)  {
    int idx,res;
    char strs[10];
    interrupt(0x21,(parentIdx << 8) + 0x4,filenames,src,&res);
    idx = getFilePathIdx(parentIdx, dest);
    if(idx == -2)
    {
        interrupt(0x21,(parentIdx << 8) + 0x5,filenames,dest,&res);
    }
    else 
    {
        interrupt(0x21,(idx << 8) + 0x5,filenames,src,&res);
    }
   
}

void cponly(char * filenames, char parentIdx1,char parentIdx2, char * src, char * dest)  {
    int idx,res;
    char strs[10];
    interrupt(0x21,(parentIdx1 << 8) + 0x4,filenames,src,&res);
    idx = getFilePathIdx(parentIdx2, dest);
    if(idx == -2)
    {
        interrupt(0x21,(parentIdx2 << 8) + 0x5,filenames,src,&res);
    }
    else 
    {
        interrupt(0x21,(idx << 8) + 0x5,filenames,src,&res);
    }
}

void cp(char * buf, char parentIdx1 , char parentIdx2, char * src, char * dest) {
    char files[1024];
    int i=0,total = 0,j = 0;
    int idxSrc,idxDest;
    char childFilename[14];
    char listFiles[64];
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);

    idxSrc = getFilePathIdx(parentIdx1, src);
    idxDest = getFilePathIdx(parentIdx2, dest);
    createFolder(src,idxDest);
    for(i=0;i<64;i++){
        if(files[i*16]==idxSrc){
            strslice(files+i*16,childFilename,2,16);
            if(files[i*16 +2] != '\0')
            {
                if(files[i*16 + 1] == 0xFF)
                {
                    cp( buf, idxSrc , idxDest,  childFilename, src);
                
                }
                else
                {
                    cponly( buf, idxSrc, idxDest,  childFilename, src);
                }
                clear(childFilename,14);
            }
            
        }
    }
    

}
// ini buat manggilnya ntar
// cp(buf, parentIdx, parentIdx, command[2], command[3]);