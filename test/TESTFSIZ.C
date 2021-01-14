#include <stdio.h>


char buffer[257];

int main() {
    FILE *f;   
    memset(buffer,' ',sizeof(buffer));

    buffer[0]='0';
    buffer[127]='1';
    buffer[255]='2';
    buffer[256]='E';
    f=fopen("test32.dat","w"); 
    if(f) {
        fwrite(buffer,1,32,f);
        fclose(f);
    }
    f=fopen("test128.dat","w"); 
    if(f) {
        fwrite(buffer,1,128,f);
        fclose(f);
    }
    f=fopen("test256.dat","w"); 
    if(f) {
        fwrite(buffer,1,256,f);
        fclose(f);
    }
    f=fopen("test257.dat","w"); 
    if(f) {
        fwrite(buffer,1,257,f);
        fclose(f);
    }
    return 0;
}
