void cwd(int pathIdx,char *dir);

int cd(int currParentIdx, char *dirPath);

void ls(unsigned char parentIndex);

void autoComplete(char *filename, char parentIdx);

void messageArguments(char *argv,char parentIndex);

void shellState(char currParentIdx, int historyCount, char *history);

void getShellState(char *argv);

void removeShellState();

int main(){
    int i, j, commandCount, historyCount, historyIdx = 0, idx, flag;
    int tabPressed = 0, arrowPressed = 0;
    char input[512];
    char filename[14];
    char temp[512];
    char history[512];
    char command[8][64];
    int parentIdx = 0xFF;
    int targetDir;
    char dir[128];
    char currShellState[8192];
    char debugOutput[512];

    char execStatus[16];

    clear(input,512);
    clear(currShellState, 8192);
    clear(history, 512);

    getShellState(currShellState);

    parentIdx = currShellState[0];
    historyCount = currShellState[1];
    
    i = 2; j = 0; idx = 0;
    while(currShellState[i] != 0x0) {
        if(currShellState[i] == '|') {
            idx++;
            i++;
            j = 0;
        } else {
            history[idx*128+j] = currShellState[i];
            i++;
            j++;
        }
    }

    while(1){
        if(!tabPressed && !arrowPressed) {
            cwd(parentIdx,dir);
        }

        interrupt(0x21,1,input,0,0);
        commandCount = strsplit(input,' ',command);

        if((input[0] == 0x00 && input[1] != 0x00)) {
            // arrow down
            if(input[1] == 0x50)
            {
                if(historyIdx > 0 )
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
                interrupt(0x21, 0, history+(historyCount-historyIdx)*128, 0, 0);
                strcpy(temp, history+(historyCount-historyIdx)*128);
                input[0] = 0xFF;
                input[1] = 0xFF;
                temp[strlen(history+(historyCount-historyIdx)*128)] = 0x0;
                strslice(temp, input + 2, 0, strlen(temp));
                input[2+strlen(temp)] = 0x0;
            }
            else
            {
                clear(input,512);
            }
            arrowPressed = 1;
        } else {
            interrupt(0x21,0,"\r\n",0,0);
            arrowPressed = 0;
            // add history
            if(historyCount < 4) {
                strcpy(history+historyCount*128, input);
                shellState((unsigned char) parentIdx, historyCount+1, history);
                historyCount++;
            } else {
                for(i=0; i<3; i++) {
                    clear(history+i*128, 128);
                    strcpy(history+i*128, history+((i+1)*128));
                    shellState((unsigned char) parentIdx, historyCount, history);
                }
                clear(history+4*128, 128);
                strcpy(history+4*128, input);
                shellState((unsigned char) parentIdx, historyCount, history);
            }

            if(strcmp(command[0], "cd", strlen(command[0])) && strlen(command[0])==2){
                targetDir = cd(parentIdx,command[1]);
                if(targetDir != -1) {
                    parentIdx = targetDir;
                } else {
                    interrupt(0x21,0, "No such directory\r\n",0,0);
                }
            } else if(strcmp(command[0],"ls",strlen(command[0])) && strlen(command[0])==2){
                ls(parentIdx);
            } else if(command[0][0] == '.' && command[0][1] == '/') {
                clear(filename, 14);
                strcpy(filename, command[0]+2);
                flag = getFilePathIdx(parentIdx, filename);
                if(flag != -2) {
                    messageArguments(filename,parentIdx);
                    interrupt(0x21,0x0006,filename,0x3000,execStatus);
                } else {
                    interrupt(0x21, 0, "File not found!", 0, 0);
                }
            } else{
                messageArguments(input,parentIdx);
                interrupt(0x21,0x0006,command[0],0x3000,execStatus);
            }
            clear(input,512);
            historyIdx = 0;
        }
    }
    return 0;
}

void cwd(int pathIdx, char *dir) {
    int depth = 0;
    char pathOrder[64];
    char absolutePath[512];
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

void ls(unsigned char parentIndex)
{
  char files[1024];
  char listFiles[64];
  char name[14];
  int i = 0,j = 0,total;
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

    interrupt(0x21, 0, name, 0, 0);
    interrupt(0x21, 0, "\n", 0, 0);
    interrupt(0x21, 0, "\r", 0, 0);
  }
}

// void autoComplete(char *filename, char parentIdx) {
//     char files[512 * 2];
//     char *temp;
//     char *currFilename;
//     int i, idxFilename, idxTemp;
//     interrupt(0x21, 2, files, 0x101, 0);
//     interrupt(0x21, 2, files + 512, 0x102, 0);

//     i = 0;
//     while(i < 1024) {
//         strslice(files, currFilename, i+2, i+16);
//         if(files[i] == parentIdx && strcmp(filename, currFilename, strlen(filename))) {
//             strslice(files, temp, i+2+strlen(filename), i+16);
//             interrupt(0x21, 0, temp, 0, 0);

//             //update filename
//             idxFilename = strlen(filename);
//             idxTemp = 0;
//             while(*(temp+idxTemp) != 0x0) {
//                 *(filename+idxFilename) = *(temp+idxTemp);
//                 idxFilename++;
//                 idxTemp++;
//             }
//             *(filename+idxFilename) = 0x0;
//             break;
//         }
//         i += 16;
//     }
// }

void messageArguments(char *argv,char parentIndex){
    char files[1024];
    char debugOutput[16];
    char trash = 0;
    int i=0;
    int argc=2;

    char arg[512];
    clear(arg,512);

    for(i=0;i<512;i++){
        if(argv[i]==' '){
            argc++;
        }
    }

    arg[0] = argc;
    arg[1] = ' ';

    arg[2] = parentIndex;
    arg[3] = ' ';

    for(i=4;i<512;i++){
        arg[i] = argv[i-4];
    }

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x5,arg,"~temp",&trash);
}

void shellState(char currParentIndex, int historyCount, char *history){
    char files[1024];

    char trash = 0;
    int i, j, idx = 0;

    char arg[512];
    clear(arg,512);

    removeShellState();

    arg[0] = currParentIndex;
    arg[1] = (unsigned char) historyCount;
    
    i = 2;
    while(idx < historyCount) {
        for(j = 0; j < strlen(history+(idx*128)); j++) {
            arg[i] = history[idx*128+j];
            i++;
        }
        arg[i] = '|';
        i++;
        idx++;
    }

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x5,arg,"~shell",&trash);
}

void getShellState(char *argv) {
    char files[1024];
    char debugOutput[16];
    int flag = 0;
    int i=0;

    interrupt(0x21,2,files,0x101,0);
    interrupt(0x21,2,files+512,0x102,0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            break;
        }
        i++;
    }

    interrupt(0x21,(i << 8) + 0x4,argv,"~shell",&flag);

    if(flag == -1) {
        argv[0] = 0xFF;
        argv[1] = 0;
        argv[2] = '|';
    }
}

void removeShellState() {
    char files[1024];
    char sectors[512];
    char map[512];
    char debugOutput[64];
    int i, j;
    int tmpIdx;

    interrupt(0x21, 2, map, 0x100, 0);
    interrupt(0x21, 2, files, 0x101, 0);
    interrupt(0x21, 2, files+512, 0x102, 0);
    interrupt(0x21, 2, sectors, 0x103, 0);

    i=0;
    while(i<64){
        if(files[i*16]==0xFF && files[i*16+1]==0xFF && files[i*16+2]=='t' && files[i*16+3]=='m' && files[i*16+4]=='p' && files[i*16+5]==0x0){
            tmpIdx = i;
            break;
        }
        i++;
    }

    i=0;
    while(i<64){
        if(files[i*16]==(unsigned char) tmpIdx && files[i*16+1]!=0xFF && files[i*16+2]=='~' && files[i*16+3]=='s' && files[i*16+4]=='h' && files[i*16+5]=='e' && files[i*16+6]=='l' && files[i*16+7]=='l' && files[i*16+8]==0x0){
            for(j=0;j<16;j++){
                if(sectors[files[i*16+1]*16+j] != 0x0 && map[sectors[files[i*16+1]*16+j]] != 0x0){
                    map[sectors[files[i*16+1]*16+j]] = 0x0;
                    sectors[files[i*16+1]*16+j] = 0x0;
                }
            }
            for(j=0;j<16;j++){
                files[i*16+j] = 0x0;
            }
            break;
        }
        i++;
    }

    interrupt(0x21, 3, map, 0x100, 0);
    interrupt(0x21, 3, files, 0x101, 0);
    interrupt(0x21, 3, files+512, 0x102, 0);
    interrupt(0x21, 3, sectors, 0x103, 0);   
}