#include <cpm.h>
#include <stdio.h>
#include <overlay.h>

int main(int argc, char ** argv)
{
    intptr_t i;
    printf("overlay test\n");
    i=ovrload("testovr1",0);
    printf("overlay return is %d\n",i);
    i=ovrload("testovr1",0);
    printf("overlay return is %d\n",i);
    i=ovrload("testovr2",0);
    printf("overlay return is %d\n",i);
    i=ovrload("testovr1",0);
    printf("overlay return is %d\n",i);
    return 0;
}

