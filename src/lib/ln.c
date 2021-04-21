#include "../module/folderIO.h"
#include "../module/fileIO.h"
#include "../module/stringutil.h"
#include "../module/math.h"

int ln(char *filepath, char *filelink,int soft,unsigned char parentIndex);

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

    if(argc==5){
        interrupt(0x21,0,"ln soft dipanggil\r\n",0,0);
        ln(command[4],command[5],1,parentIdx);
    } else if(argc==4){
        interrupt(0x21,0,"ln hard dipanggil\r\n",0,0);
        ln(command[3],command[4],0,parentIdx);
    } else{
        interrupt(0x21,0,"Usage : ln <source> <dest>\r\n");
    }

    deleteFile(getFilePathIdx(0xFF,"tmp/~temp"));
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

//usage : ln -s original_file linked_file
// status code : 0 (success), -1 (original_file error) , -2 (linked_file error), -3 (sector full)
// soft calculation files index = files[i+1] - 0x1F (if files[i+1]>=x20)
int ln(char *filepath, char *filelink,int soft,unsigned char parentIndex){
    char files[1024];
    char sectors[512];
    int i,j;
    int idx;
    // char output[100];
    char filepathmatrix[16][64];
    char filelinkmatrix[16][64];
    int charcount,linkcount;
    char res[100];
    char filename[14];

    clear(res,100);
    clear(filename,14);

    interrupt(0x21,2,files,0x101,0,0);
    interrupt(0x21,2,files+512,0x102,0,0);

    if(soft){
        idx = getFilePathIdx(parentIndex,filepath);
        if((getFilePathIdx(parentIndex, filepath)==-1 || getFilePathIdx(parentIndex,filepath)==-2)){
            interrupt(0x21,0, "Original file not found\r\n",0,0);
            return -1;
        }
        if(getFilePathIdx(parentIndex, filelink)>=0){
            interrupt(0x21,0, "File already exists!\r\n",0,0);
            return -2;
        }
        if(getFilePathIdx(parentIndex,filelink)!=-2){
            interrupt(0x21,0,"Destination folder doesn't exist\r\n");
            return -2;
        }
        // IF everything is OK

        linkcount = strsplit(filelink, '/', filelinkmatrix);
        charcount =0;
        for(i=0;i<linkcount;i++){
            j=0;
            while(j<64 && filelinkmatrix[i][j]){
                charcount++;
                j++;
            }
        }
        for(i=charcount;i>=0;i--){
            if(filelink[i]=='/'){
                break;
            }
        }

        if(i==-1 || linkcount==1){
            strslice(filelink,filename,0,16);
        } else{
            strslice(filelink,res,0,i);
            strslice(filelink,filename,i+1,16);
        }

        for(i=0;i<1024;i+=16){
            if(isempty(files+i,16)){
                files[i] = linkcount == 1 ? parentIndex : getPathIdx(parentIndex,res);
                files[i+1] = 0x20 + idx;
                for(j=2;j<16;j++){
                    files[i+j] = filename[j-2];
                }
                interrupt(0x21,3,files,0x101,0,0);
                interrupt(0x21,3,files+512,0x102,0,0);
                return 0;
            }
        }
        interrupt(0x21,0,"cannot create file as files sector is already full\r\n",0,0);
        return -3;
    } else{
        idx = getFilePathIdx(parentIndex,filepath);
    
        if((getFilePathIdx(parentIndex, filepath)==-1 || getFilePathIdx(parentIndex,filepath)==-2)){
            interrupt(0x21,0, "Original file not found\r\n",0,0);
            return -1;
        }
        if(getFilePathIdx(parentIndex, filelink)>=0){
            interrupt(0x21,0, "File already exists!\r\n",0,0);
            return -2;
        }
        if(getFilePathIdx(parentIndex,filelink)!=-2){
            interrupt(0x21,0,"Destination folder doesn't exist\r\n");
            return -2;
        }
        // IF everything is OK

        linkcount = strsplit(filelink, '/', filelinkmatrix);

        charcount =0;
        for(i=0;i<linkcount;i++){
            j=0;
            while(j<64 && filelinkmatrix[i][j]){
                charcount++;
                j++;
            }
        }
        for(i=charcount;i>=0;i--){
            if(filelink[i]=='/'){
                break;
            }
        }

        if(i==-1 || linkcount==1){
            strslice(filelink,filename,0,16);
        } else{
            strslice(filelink,res,0,i);
            strslice(filelink,filename,i+1,16);
        }

        for(i=0;i<1024;i+=16){
            if(isempty(files+i,16)){
                files[i] = linkcount == 1 ? parentIndex : getPathIdx(parentIndex,res);

                interrupt(0x21,0,res,0,0);
                interrupt(0x21,0,"\r\n",0,0);
                files[i+1] = files[idx*16+1];
                for(j=2;j<16;j++){
                    files[i+j] = filename[j-2];
                }
                interrupt(0x21,3,files,0x101,0,0);
                interrupt(0x21,3,files+512,0x102,0,0);
                return 0;
            }
        }
        interrupt(0x21,0,"cannot create file as files sector is already full\r\n",0,0);
        return -3;
    }
    return 0;
}