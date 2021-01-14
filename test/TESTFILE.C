#include <stdio.h>
#include <stdlib.h>
#include <unixio.h>

int main() {
    FILE * f;
    int i;
    fprintf(stderr,"create file...\n");
    f=fopen("test.dat","w");
    if(!f) {
        fprintf(stderr,"cannot create file (%d)\n",errno);
        return -1;
    }
    fprintf(f,"Hello\n");
    fclose(f);
    i=rename("test.dat","testx.dat");
    fprintf(stderr,"rename file (%d)\n",i);
    i=unlink("testx.dat");
    fprintf(stderr,"delete file (%d)\n",i);
    i=unlink("testx.dat");
    fprintf(stderr,"delete file (%d)\n",i);
    fprintf(stderr,"create file...\n");
    f=fopen("test.dat","w");
    if(!f) {
        fprintf(stderr,"cannot create file (%d)\n",errno);
        return -1;
    }
    fprintf(f,"Hello\n");
    fclose(f);
    return 0;
}
