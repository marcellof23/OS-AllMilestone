#include "shell.h"
#include "utilities.h"

void cwd(int pathIdx, char *dir) {
    int depth = 0;
    char pathOrder[64];
    char *absolutePath;
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

void lsList(unsigned char parentIndex,int * countFile, char * listFiles)
{
  char files[1024];
  int i = 0,j = 0;
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
  *countFile = j;
  listFiles[i] = 0xFF;
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
            interrupt(0x21, 4 + 0x100*(files[0x10*(files[0x10*pathIdx+1]-0x20)]), buff, linkedname, 0);
            interrupt(0x21, 0, buff , 0, 0);
            interrupt(0x21,0,"\r\n", 0,0);
            return;
        }       
    }
    interrupt(0x21, 0, "file tidak ditemukan\r\n", 0, 0);
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
void mkdir( char *filenames,unsigned char parentIndex)
{
    char files[1024];
    int i = 0, InitIdx;
    int j = 0;
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);
    while(i<0x40)
    {
        if(files[i * 0x10] == parentIndex && (strcmp(filenames,files + (i * 16) + 2 ,14) == 1) )
        {
            // interrupt(0x21, 0, "Folder sudah ada \r\n", 0, 0);
            return;
        }
        i++;
    }

    
    while(files[j*0x10+2] != '\0' )
    {
        j++;
    } 
    // if(j < 0 || j >= 64)
    // {
    //     interrupt(0x21, 0, "Sudah tidak bisa ditambah \r\n", 0, 0);
    // }
    // else
    // {
        InitIdx = j * 0x10;
        files[InitIdx] = parentIndex;
        files[InitIdx + 1] = 0xFF;
        for(i = 0 ;i < 14; i++)
        {
            if(filenames[i] == '\0')
                break;
            files[InitIdx + i + 2] = filenames[i];
        }
    // }
    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files+512, 0x102, 0);

    // interrupt(0x21, 0, filenames, 0, 0);
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

    if(files[idx*16+1]==0xFF)
    {
        interrupt(0x21,0,"It's a folder\r\n",0,0);
        
        for(i=0;i<64;i++){
            if(files[i*16]==idx){
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

    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);

    idx = getFilePathIdx(parentIndex, filename);

    for(i=0;i<64;i++){
        if(files[i*16]==idx){
            strslice(files+i*16,childFilename,2,16);
            interrupt(0x21,0,childFilename,0,0);
            interrupt(0x21,0,"\r\n",0,0);
            rmRecursive(childFilename, idx);
            clear(childFilename,14);
        }
    }

    rm(filename,parentIndex);
}

void cpFiles(char * filenames, char parentIdx, char * src, char * dest)  {
    int idx,res;
    char strs[10];
    readFile(filenames,src,&res, parentIdx);
    idx = getFilePathIdx(parentIdx, dest);
    if(idx == -2)
    {
        writeFile(filenames,dest,&res, parentIdx);
    }
    else 
    {
        writeFile(filenames,src,&res, idx);
    }
   
}

void cp(char * filenames, char parentIdx, char * src, char * dest) {
    char files[1024];
    int idxFolder,n,i,row,cnt,idxList;
    char nama[14];
    char folderList[64];
    char curFile[14];
    int isFolder = 0,folderExist = 0;
    int itr = 0;
    readSector(files, 0x101);
    readSector(files+512, 0x102);

    
    
    idxFolder = getPathIdx(parentIdx, dest);
    idxList = getPathIdx(parentIdx, src);

    for(row = 0;row<64;row++) 
    {
        if((files[row*16] == idxList) && (files[row * 16 + 2] != 0x0) )
        {
            cnt++;
        }
    }
    n = cnt;
    // mkdir(src,idxFolder);
    
    // for(i = 0; i<n;i++) 
    // {
        itr = 0;
        while(itr < 1024) {
            readSector(files, 0x101);
            readSector(files+512, 0x102);
            strslice(files, nama, itr+2, itr+16);
            if(strcmp(src, nama, strlen(filenames)) && strlen(src) == strlen(nama)) {
                if((unsigned char)files[itr+1] == 0xFF) {
                    interrupt(0x21,0,nama,0,0);
                    cp(filenames,idxFolder,nama,src);
                }
                else 
                {
                    cpFiles(filenames,idxFolder,nama,src);
                }
            }
            itr+= 16;
        }
        
    // }

}

void cpRecursive(char * filenames, char parentIdx, char * src, char * dest){
    char files[1024];
    int idxFolder,n,i,j = 0,row,total = 0,idxList,x,y;
    char name[14];
    char listFiles[64],strs[10];
    int idxDest = 0, idxSource = 0,folderExist = 0,copyName = 0;
    int itr = 0;
    readSector(files, 0x101);
    readSector(files+512, 0x102);
    idxSource = getPathIdx(parentIdx, src);
    idxDest= getFilePathIdx(parentIdx, dest);
    
    while(i<64)
    {
        if(files[i*0x10] == idxSource && files[i*0x10 + 2] != '\0')
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
        copyName = getFilePathIdx(idxSource, name);
        // interrupt(0x21,0,name,0,0);
        if((unsigned char)files[copyName * 16 + 1] == 0xFF) 
        {   
            y = 0;
            while(files[y*0x10+2] != '\0' )
            {
                y++;
            } 
            files[y * 0x10] = idxDest;
            files[y * 0x10 + 1] = 0xFF;
            for(x = 0 ;x < 14; x++)
            {
                if(name[x] == '\0')
                    break;
                files[y * 0x10 + x + 2] = name[x];
            }
            interrupt(0x21, 3, files, 0x101, 0);
            interrupt(0x21, 3, files+512, 0x102, 0);
            
            interrupt(0x21,0,"RECURSIVE DIPANGGIL\r\n",0,0);
            cpRecursive(filenames,y,name,dest);
        }
        else
        {
            cpFiles(filenames,parentIdx,name,dest);
        }
    }
}

void shell(){
    int i, j, commandCount, historyCount = -1, historyIdx = -1, count, idx;
    int tabPressed = 0, arrowPressed = 0;
    char input[128];
    char temp[128];
    char history[4][128];
    char command[8][64];
    int parentIdx = 0xFF;
    int targetDir,idxS,idxD;
    char dir[128];
    char buf[512 * 16];
    int status;
    char strs[10];
        
    for(i = 0; i < 4; i++) {
        clear(history[i], 128);
    }

    while(1){
        if(!tabPressed && !arrowPressed) {
            cwd(parentIdx,dir);
        }

        interrupt(0x21,1,input,0,0);
        commandCount = strsplit(input,' ',command);

        if(*(command[commandCount-1]+strlen(command[commandCount-1])-1) == 0x09) {

            //catet posisi terakhir input
            count = 0;
            for(i = 0; i < commandCount; i++) {
                count += strlen(command[i]);
            }
            count += commandCount - 2;

            //catet posisi terakhir command
            j = strlen(command[commandCount-1])-1;

            *(command[commandCount-1]+strlen(command[commandCount-1])-1) = 0x0;
            autoComplete(command[commandCount-1], parentIdx);
            
            //update input
            i = count;
            while(j < strlen(command[commandCount-1])) {
                *(input+i) = *(command[commandCount-1]+j);
                i++;
                j++;
            }

            *(input+i) = 0x0;
            for(idx = 0; idx < strlen(input); idx++) {
                *(temp+idx) = *(input+idx);
            }
            *(temp+strlen(input)) = 0x0;

            //overwrite input, add payload to input[0] and input[1]
            *(input) = 0xFF;
            *(input+1) = 0xFF;
            strslice(temp, input+2, 0, strlen(temp));
            *(input+2+strlen(temp)) = 0x0;
            tabPressed = 1;
        } else if((input[0] == 0x00 && input[1] != 0x00)) {

            // arrow down
            if(input[1] == 0x50)
            {
                if(historyIdx > -1 )
                {
                    historyIdx = historyIdx - 1;
                }
            }
            else if(input[1] == 0x48)
            {
                if(historyIdx < historyCount)
                {
                    historyIdx = historyIdx + 1;
                }
            }
            else 
            {
                interrupt(0x21, 0, "command not found", 0, 0);
            }
            if(historyIdx != -1)
            {
                interrupt(0x21, 0, history[historyIdx], 0, 0);
                strslice(history[historyIdx], temp, 0, strlen(history[historyIdx]));
                *(input) = 0xFF;
                *(input+1) = 0xFF;
                *(temp+strlen(history[historyIdx])) = 0x0;
                strslice(temp, input + 2 ,0 , strlen(temp));
                *(input+2+strlen(temp)) = 0x0;
            }
            else
            {
                clear(input,128);
            }
            arrowPressed = 1;
        } else {
            if(strcmp(command[0], "cd", strlen(command[0])) && strlen(command[0])==2){
                targetDir = cd(parentIdx,command[1]);
                if(targetDir != -1) {
                    parentIdx = targetDir;
                } else {
                    interrupt(0x21,0, "No such directory\r\n",0,0);
                }
            } else if(strcmp(command[0], "ls", strlen(command[0])) && strlen(command[0])==2 ){
                ls((unsigned char)parentIdx);
            } else if(strcmp(command[0],"cat",strlen(command[0])) && strlen(command[0])==3 ){
                // interrupt(0x21,0, command[1],0,0);
                cat(command[1],parentIdx);
            } else if(strcmp(command[0],"mkdir",strlen(command[0])) && strlen(command[0])==5){
                mkdir(command[1],parentIdx);
            } else if(strcmp(command[0],"ln",strlen(command[0])) && strlen(command[0])==2){
                if(strcmp(command[1],"-s",strlen(command[1])) && strlen(command[1])==2){
                    status = ln(command[2],command[3],1,parentIdx);
                } else{
                    status = ln(command[1],command[2],0,parentIdx);
                }
            } 
            else if(strcmp(command[0],"rm",strlen(command[0])) && strlen(command[0])==2) 
            {
                if(strcmp(command[1],"-r",strlen(command[1])) && strlen(command[1])==2){
                    rmRecursive(command[2], parentIdx);
                } else{
                    rm(command[1],parentIdx);
                }
            }
            else if(strcmp(command[0],"mv",strlen(command[0])) && strlen(command[0])==2) 
            {
                mv(command[1], command[2], parentIdx);
            }
            else if(strcmp(command[0],"cp",strlen(command[0])) && strlen(command[0])==2) 
            {
                
                if(strcmp(command[1],"-r",strlen(command[1])) && strlen(command[1])==2){
                    cpRecursive(buf, parentIdx, command[2], command[3]);
                } else{
                    idxS = getFilePathIdx(parentIdx, command[1]);
                    idxD = getFilePathIdx(parentIdx, command[2]);
                    // clear(strs,10);
                    // itoa(idxS,10,strs);
                    // interrupt(0x21,0,strs,0,0);
                    // clear(strs,10);
                    // itoa(idxD,10,strs);
                    // interrupt(0x21,0,strs,0,0);
                    if(idxS == idxD && strcmp(command[1], command[2], 14))
                    {
                        interrupt(0x21,0,"You copy two file with same name\r\n",0,0);
                    }
                    else
                    {
                        cpFiles(buf, parentIdx, command[1], command[2]);
                    }
                    
                }
            }
            else{
                interrupt(0x21,0, command[0],0,0);
                interrupt(0x21,0," not found\r\n",0,0);
            }

            //simpen command ke history
            // if(historyCount < 3) {
            //     historyCount++;
            //     strslice(input, history[historyCount], 0, strlen(input));
            //     *(history[historyCount] + strlen(input)) = 0x0;
            // } else {
            //     historyIdx = 0;
            //     while(historyIdx < 3) {
            //         strslice(history[historyIdx+1], history[historyIdx], 0, strlen(history[historyIdx+1]));
            //         *(history[historyIdx] + strlen(history[historyIdx+1])) = 0x0;
            //         historyIdx++;
            //     }
            //     strslice(input, history[3], 0, strlen(input));
            //     *(history[3] + strlen(input)) = 0x0;
            //     historyCount = 3;
            // }
            // historyIdx=-1;
            // tabPressed = 0;
            // arrowPressed = 0;
            for(i = 2 ;i>=0 ;i--)
            {
                strslice(history[i], history[i+1], 0, strlen(history[i]));
                *(history[i+1]+strlen(history[i])) = 0x0;
            }
            if(historyCount < 3) {
                historyCount++;
            }
            strslice(input, history[0], 0, strlen(input));
            *(history[0]+strlen(input)) = 0x0;
            historyIdx=-1;
            tabPressed = 0;
            arrowPressed = 0;
            clear(input,128);
        }
        
    }
}