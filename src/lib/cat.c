int main(){
    char execStatus[16];
    interrupt(0x21,0,"cat dipanggil\r\n",0,0);
    interrupt(0x21,0xFF06,"shell",0x3000,execStatus);
    return 1;
}

// void cat(char * filenames, unsigned char parentIdx)
// {
//     char files[1024];
//     char buff[512 * 16];
//     int idx = strlen(filenames);
//     int pathIdx;
//     char output[100];
//     char filematrix[64][64];
//     char countsplit;
//     char linkedname[17];
//     // char * currfile;
//     interrupt(0x21, 2, files, 0x101, 0);
//     interrupt(0x21, 2, files+512, 0x102, 0);
//     while(idx < 14) {
//         *(filenames + idx) = 0x0;
//         idx++;
//     }
//     clear(output,100);
//     pathIdx = getFilePathIdx(parentIdx, filenames);
//     countsplit = strsplit(filenames, '/', filematrix);

//     if(files[0x10*pathIdx+1]<0x20){
//         if(pathIdx>=0)
//         {
//             interrupt(0x21, 4 + 0x100*files[0x10*pathIdx], buff, filematrix[countsplit-1], 0);
//             interrupt(0x21, 0, buff , 0, 0);
//             interrupt(0x21, 0, "\r\n" , 0, 0);
//             return;
//         }
//     } else{
//         if(pathIdx>=0)
//         {
//             strslice(files+0x10*(files[0x10*pathIdx+1]-0x20),linkedname,2,16);
//             interrupt(0x21, 4 + 0x100*(files[0x10*(files[0x10*pathIdx+1]-0x20)]), buff, linkedname, 0);
//             interrupt(0x21, 0, buff , 0, 0);
//             interrupt(0x21,0,"\r\n", 0,0);
//             return;
//         }       
//     }
//     interrupt(0x21, 0, "file tidak ditemukan\r\n", 0, 0);
// }