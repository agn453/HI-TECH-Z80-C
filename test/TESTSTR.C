#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *v[]={
    "test","Test",
    "test","test",
    "test1","test",
    "test","test1",
    "rest","test",
    0
};

int main() {
    int i=0;
    printf("strcmp test:\n");
    while(v[i]) {
        printf("- %s<>%s=%d\n",v[i],v[i+1],strcmp(v[i],v[i+1]));
        i=i+2;
    }
    i=0;
    printf("stricmp test:\n");
    while(v[i]) {
        printf("- %s<>%s=%d\n",v[i],v[i+1],stricmp(v[i],v[i+1]));
        i=i+2;
    }
    printf("strnicmp test:\n");
    printf("- %s<>%s=%d\n","test","test1",strnicmp("test","test1",strlen("test")));
    printf("- %s<>%s=%d\n","test1","test",strnicmp("test1","test",strlen("test")));
    printf("- %s<>%s=%d\n","Test1","test",strnicmp("Test1","test",strlen("test")));
    return 0;
}
