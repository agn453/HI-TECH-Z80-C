/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO  -S
%DELETE $1.CRL 
*/
/*********************************************************************
*                               TEE                                  *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
* Converted to Hi-Tech C by John Elliott, 17 November 1998           *
*                                                                    *
* Build using   C -O -C PIPEMGR.C                                    *
*               C -O TEE.C PIPEMGR.OBJ -LC ! GENCOM TEE PIPEMGR      *
*               ERA TEE.OBJ                                          *
*               ERA PIPEMGR.OBJ                                      *
*                                                                    *
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pipemgr.h"
    
char *version;
    
int main(int argc, char **argv)
{
    FILE *fp;
    int c;
    char teeflag;
    char fname[MAXLINE];

    version = "Version 3.0";
    
    init_args(&argc, &argv);

    if (argc >= 2)
    {
        strcpy(fname, *++argv);
        teeflag = TRUE;
        fp = fopen(fname, "w");
        if (!fp)
        {
            fprintf(stderr,"TEE: Unable to create file \'%s\'.",fname);
            exit(1);
        }
    }
    else
        teeflag = FALSE;

    while ((c=getchar()) != EOF) 
    {
        putchar(c);
        if (teeflag)
            putc(c,fp);
    }
    fflush(fp);
    fclose(fp);
    exit(0);
}
