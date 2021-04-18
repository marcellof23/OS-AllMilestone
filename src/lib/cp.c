int main(){
    char execStatus[16];
    interrupt(0x21,0,"Cp dipanggil\r\n",0,0);
    interrupt(0x21,0x0006,"shell",0x3000,execStatus);
    return 1;
}