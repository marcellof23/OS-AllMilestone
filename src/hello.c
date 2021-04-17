int main(){
    interrupt(0x10,0xe00 + 'G',0,0,0);
    while(1);
    return 0;
}

