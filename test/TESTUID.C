#include "stdio.h"
#include "cpm.h"
#include <stdio.h>

#include <stdio.h>
#include <sys.h>

int main(int argc, char ** argv)
{
    printf("getuid=%d\n",getuid());
    printf("setuid=%d\n",setuid(3));
    printf("getuid=%d\n",getuid());
    printf("getuid=%d\n",getuid());
    return 0;
}

