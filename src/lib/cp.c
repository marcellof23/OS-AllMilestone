int main(){
    char execStatus[16];
    interrupt(0x21,0,"Cp dipanggil\r\n",0,0);
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

// void cpFiles(char * filenames, char parentIdx, char * src, char * dest)  {
//     int idx,res;
//     readFile(filenames,src,&res, parentIdx);
//     idx = getPathIdx(parentIdx, dest);
//     writeFile(filenames,src,&res, idx);
// }

// void cp(char * filenames, char parentIdx, char * src, char * dest) {
//     char files[1024];
//     int idxFolder,n,i,row,cnt;
//     char nama[14];
//     char curFile[14];
//     int isFolder = 0,folderExist = 0;
//     int itr = 0;
//     readSector(files, 0x101);
//     readSector(files+512, 0x102);

//     while(itr < 1024) {
//         strslice(files, curFile, itr+2, itr+16);
//         if(strcmp(src, curFile, strlen(filenames)) && strlen(src) == strlen(curFile)) {
//             if((unsigned char)files[itr+1] == 0xFF) {
//                 isFolder = 1;
//             }
//         }
//         itr+= 16;
//     }
    
//     for(row = 0;row<64;row++) 
//     {
//         if((files[row*16] == parentIdx) && (files[row * 16 + 2] != 0x0) )
//         {
//             cnt++;
//         }
//     }
//     n = cnt;
//     idxFolder = getPathIdx(parentIdx, dest);
//     mkdir(src,idxFolder);
//     // i = 0;
//     // while(i<0x40)
//     // {
//     //     if(files[i * 0x10] == parentIdx && (strcmp(dest,files + (i * 16) + 2 ,14) == 1) )
//     //     {
//     //         folderExist = 1;
//     //         break;
//     //     }
//     //     i++;
//     // }
//     // if(folderExist)
//     // {

//     // }
//     // else
//     // {
//             //mkdir
//     // }
        
//     for(i = 0; i<n;i++) {
//         if(!isFolder) {
//             cpFiles(filenames,idxFolder,nama,src);
//         }
//         else {
//             cp(filenames,idxFolder,nama,src);
//         }
//     }

// }

// void cpRecursive(char * filenames, char parentIdx, char * src, char * dest){
//     char files[1024];
//     int i=0;
//     int idx,idxsrc,idxdest;
//     char childFilename[14],char1[14],char2[14];

//     readSector(files, 0x101);
//     readSector(files+512, 0x102);

//     idx = getFilePathIdx(parentIdx, filenames);
//     idxsrc = getFilePathIdx(parentIdx, src);
//     idxdest = getFilePathIdx(parentIdx, dest);
//     for(i=0;i<64;i++){
//         if(files[i*16]==idxsrc){
//             strslice(files+i*16,char1,2,16);
//         }
//     }
//     for(i=0;i<64;i++){
//         if(files[i*16]==idxdest){
//             strslice(files+i*16,char2,2,16);
//         }
//     }
//     for(i=0;i<64;i++){
//         if(files[i*16]==idx){
//             strslice(files+i*16,childFilename,2,16);
//             interrupt(0x21,0,childFilename,0,0);
//             interrupt(0x21,0,"\r\n",0,0);
//             cpRecursive(childFilename, idx, char1, char2);
//             clear(childFilename,14);
//         }
//     }
//     cp(filenames, idx, char1, char2);
// }