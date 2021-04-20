int main(){
    char execStatus[16];
    interrupt(0x21,0,"Cp dipanggil\r\n",0,0);
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

// void cpFiles(char * filenames, char parentIdx, char * src, char * dest)  {
//     int idx,res;
//     char strs[10];
//     readFile(filenames,src,&res, parentIdx);
//     idx = getFilePathIdx(parentIdx, dest);
//     if(idx == -2)
//     {
//         writeFile(filenames,dest,&res, parentIdx);
//     }
//     else 
//     {
//         writeFile(filenames,src,&res, idx);
//     }
   
// }

// void cponly(char * filenames, char parentIdx1,char parentIdx2, char * src, char * dest)  {
//     int idx,res;
//     char strs[10];
//     readFile(filenames,src,&res, parentIdx1);
//     idx = getFilePathIdx(parentIdx2, dest);
//     if(idx == -2)
//     {
//         writeFile(filenames,dest,&res, parentIdx2);
//     }
//     else 
//     {
//         writeFile(filenames,src,&res, idx);
//     }
// }

// void cp(char * buf, char parentIdx1 , char parentIdx2, char * src, char * dest) {
//     char files[1024];
//     int i=0,total = 0,j = 0;
//     int idxSrc,idxDest;
//     char childFilename[14];
//     char listFiles[64];
//     interrupt(0x21, 2, files, 0x101, 0);
//     interrupt(0x21, 2, files+512, 0x102, 0);

//     idxSrc = getFilePathIdx(parentIdx1, src);
//     idxDest = getFilePathIdx(parentIdx2, dest);
//     mkdir(src,idxDest);
//     for(i=0;i<64;i++){
//         if(files[i*16]==idxSrc){
//             strslice(files+i*16,childFilename,2,16);
//             interrupt(0x21,0,childFilename,0,0);
//             interrupt(0x21,0,"\r\n",0,0);
//             if(files[i*16 +2] != '\0')
//             {
//                 if(files[i*16 + 1] == 0xFF)
//                 {
//                     cp( buf, idxSrc , idxDest,  childFilename, src);
                
//                 }
//                 else
//                 {
//                     cponly( buf, idxSrc, idxDest,  childFilename, src);
//                 }
//                 clear(childFilename,14);
//             }
            
//         }
//     }
    

// }
// ini buat manggilnya ntar
// cp(buf, parentIdx, parentIdx, command[2], command[3]);