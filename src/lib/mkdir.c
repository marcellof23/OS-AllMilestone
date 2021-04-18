int main(){
    char execStatus[16];
    interrupt(0x21,0,"mkdir dipanggil\r\n",0,0);
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}

// void mkdir( char *filenames,unsigned char parentIndex)
// {
//     char files[1024];
//     int i = 0, InitIdx;
//     int j = 0;
//     interrupt(0x21, 2, files, 0x101, 0);
//     interrupt(0x21, 2, files+512, 0x102, 0);
//     while(i<0x40)
//     {
//         if(files[i * 0x10] == parentIndex && (strcmp(filenames,files + (i * 16) + 2 ,14) == 1) )
//         {
//             interrupt(0x21, 0, "Folder sudah ada \r\n", 0, 0);
//             return;
//         }
//         i++;
//     }
//     while(files[j*0x10+2] != '\0' )
//     {
//         j++;
//     } 
//     if(j < 0 || j >= 64)
//     {
//         interrupt(0x21, 0, "Sudah tidak bisa ditambah \r\n", 0, 0);
//     }
//     else
//     {
//         InitIdx = j * 0x10;
//         files[InitIdx] = parentIndex;
//         files[InitIdx + 1] = 0xFF;
//         for(i = 0 ;i < 14; i++)
//         {
//             if(filenames[i] == '\0')
//                 break;
//             files[InitIdx + i + 2] = filenames[i];
//         }
//     }
//     interrupt(0x21, 3, files, 0x101, 0);
//     interrupt(0x21, 3, files+512, 0x102, 0);

//     // interrupt(0x21, 0, filenames, 0, 0);
// }