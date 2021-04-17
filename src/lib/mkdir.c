int main(){
    char execStatus[16];
    interrupt(0x21,0,"mkdir dipanggil\r\n",0,0);
    interrupt(0x21,0xFF06,"shell",0x3000,execStatus);
    return 1;
}